//
// Created by li on 8/15/24.
//

#include "api/create_peerconnection_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "rtc_base/logging.h"

#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"

#include "rtc_stream/rtc_stream.h"
#include "core/xchat_global.h"

namespace mRTC {

    class SetRemoteDescriptionObserver :
            public webrtc::SetRemoteDescriptionObserverInterface {
    public:
        SetRemoteDescriptionObserver(RtcStream *stream) :
                _stream(stream) {}

        void OnSetRemoteDescriptionComplete(webrtc::RTCError err) override {
            if (!err.ok()) {
                RTC_LOG(LS_WARNING) << "set remote sdp failed: " <<
                                    err.message();
                return;
            }
            RTC_LOG(LS_INFO) << " set remote sdp succeed . ";

            _stream->createAnswer();
        }

    private:
        RtcStream *_stream;
    };

    class CreateSessionDescriptionObserver :
            public webrtc::CreateSessionDescriptionObserver {
    public:
        CreateSessionDescriptionObserver(RtcStream *stream) :
                _stream(stream) {}

    private:
        void OnSuccess(webrtc::SessionDescriptionInterface *desc) {
            RTC_LOG(LS_INFO) << " create local sdp succeed . ";
            std::string sdp;
            desc->ToString(&sdp);
            RTC_LOG(LS_VERBOSE) << " sdp desc: \n" << sdp;
            _stream->setLocalAnswer(desc);
        }

        void OnFailure(webrtc::RTCError error) {
            RTC_LOG(LS_ERROR) << " create local sdp failed: " << error.message();
        }

    private:
        RtcStream *_stream;
    };

    class SetLocalDescriptionObserver :
            public webrtc::SetLocalDescriptionObserverInterface {
    public:
        SetLocalDescriptionObserver(RtcStream *stream) :
                _stream(stream) {}

    private:
        void OnSetLocalDescriptionComplete(webrtc::RTCError error) override {
            if (!error.ok()) {
                RTC_LOG(LS_ERROR) << " set local sdp failed: " << error.message();
                return;
            }

            RTC_LOG(LS_ERROR) << " set local sdp succeed . " << error.message();
        }

    private:
        RtcStream *_stream;
    };

    RtcStream::RtcStream(const std::string uid, const std::string streamName)
            : _uid(uid),
              _streamName(streamName) {

    }

    bool RtcStream::init() {
        _pcFactory = webrtc::CreatePeerConnectionFactory(
                mRTCGlobal::getInstance()->netThread().get() /* network_thread */,
                mRTCGlobal::getInstance()->workThread().get() /* worker_thread */,
                mRTCGlobal::getInstance()->apiThread().get() /* signaling_thread */,
                nullptr /* default_adm */,
                webrtc::CreateBuiltinAudioEncoderFactory(),
                webrtc::CreateBuiltinAudioDecoderFactory(),
                //webrtc::CreateBuiltinVideoEncoderFactory(),
                //webrtc::CreateBuiltinVideoDecoderFactory(),
                std::make_unique<webrtc::VideoEncoderFactoryTemplate<
                        webrtc::LibvpxVp8EncoderTemplateAdapter,
                        webrtc::LibvpxVp9EncoderTemplateAdapter,
                        webrtc::OpenH264EncoderTemplateAdapter,
                        webrtc::LibaomAv1EncoderTemplateAdapter>>(),
                std::make_unique<webrtc::VideoDecoderFactoryTemplate<
                        webrtc::LibvpxVp8DecoderTemplateAdapter,
                        webrtc::LibvpxVp9DecoderTemplateAdapter,
                        webrtc::OpenH264DecoderTemplateAdapter,
                        webrtc::Dav1dDecoderTemplateAdapter>>(),
                nullptr /* audio_mixer */,
                nullptr /* audio_processing */);

        if (!_pcFactory) {
            RTC_LOG(LS_ERROR) << " CreatePeerConnectionFactory failed";
            return false;
        }

        webrtc::PeerConnectionInterface::RTCConfiguration cfg;
        cfg.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;

        //注册观测者
        webrtc::PeerConnectionDependencies dep(this);
        auto pcOrError = _pcFactory->CreatePeerConnectionOrError(
                cfg, std::move(dep));
        if (!pcOrError.ok()) {
            RTC_LOG(LS_ERROR) << " CreatePeerConnectionOrError failed: "
                              << pcOrError.error().message();
            return false;
        }

        _pc = pcOrError.value();
        return true;
    }

    void RtcStream::setRemoteOffer(const std::string &offer) {
        if (!_pc) {
            RTC_LOG(LS_ERROR) << " _pc is nullptr ";
            return;
        }

        absl::optional<webrtc::SdpType> sdpType = webrtc::SdpTypeFromString("offer");
        webrtc::SdpParseError err;
        std::unique_ptr<webrtc::SessionDescriptionInterface> sessionDescription =
                webrtc::CreateSessionDescription(sdpType.value(), offer, &err);
        if (!sessionDescription) {
            RTC_LOG(LS_ERROR) << " CreateSessionDescription failed: \n"
                              << " line: " << err.line << "\n"
                              << " description: " << err.description;
            return;
        }

        auto observer = rtc::make_ref_counted<SetRemoteDescriptionObserver>(this);
        _pc->SetRemoteDescription(std::move(sessionDescription), observer);
    }

    void RtcStream::createAnswer() {
        if (!_pc) {
            RTC_LOG(LS_ERROR) << " _pc is nullptr ";
            return;
        }

        auto observer = rtc::make_ref_counted<CreateSessionDescriptionObserver>(this);
        _pc->CreateAnswer(observer.get(),
                          webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    }

    void RtcStream::setLocalAnswer(webrtc::SessionDescriptionInterface *desc) {
        if (!_pc) {
            RTC_LOG(LS_ERROR) << " _pc is nullptr ";
            return;
        }

        auto observer = rtc::make_ref_counted<SetLocalDescriptionObserver>(this);

        std::unique_ptr<webrtc::SessionDescriptionInterface> answerDesc(desc);
        _pc->SetLocalDescription(std::move(answerDesc), observer);
    }

    void RtcStream::OnSignalingChange(
            webrtc::PeerConnectionInterface::SignalingState new_state) {
        RTC_LOG(LS_INFO) << " peer connection SignalingState: "
                         << _lastSignalingState << " -> " << new_state;
        _lastSignalingState = new_state;
    }

    void RtcStream::OnDataChannel(
            rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) {
    }

    void RtcStream::OnIceCandidate(
            const webrtc::IceCandidateInterface *candidate) {
        std::string strCandidate;
        candidate->ToString(&strCandidate);
        RTC_LOG(LS_INFO) << " new candidate: " << strCandidate;
    }

    void RtcStream::OnIceGatheringChange(
            webrtc::PeerConnectionInterface::IceGatheringState new_state) {
        RTC_LOG(LS_INFO) << " ice gathering change: "
                         << _lastGatheringState << " -> " << new_state;
        _lastGatheringState = new_state;
    }

    void RtcStream::OnConnectionChange(
            webrtc::PeerConnectionInterface::PeerConnectionState new_state) {

        RTC_LOG(LS_INFO) << " peer connection state change: "
                         << _lastPcState << " -> " << new_state;
        _lastPcState = new_state;

        if (new_state ==
            webrtc::PeerConnectionInterface::PeerConnectionState::kConnecting) {
            _sigPcStateChange(shared_from_this(), RtcStreamState::Connecting);
        } else if (new_state ==
                   webrtc::PeerConnectionInterface::PeerConnectionState::kConnected) {
            _sigPcStateChange(shared_from_this(), RtcStreamState::Connected);
        } else if (new_state ==
                   webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected) {
            _sigPcStateChange(shared_from_this(), RtcStreamState::Disconnected);
        } else if (new_state ==
                   webrtc::PeerConnectionInterface::PeerConnectionState::kFailed ||
                   new_state ==
                   webrtc::PeerConnectionInterface::PeerConnectionState::kClosed) {
            _sigPcStateChange(shared_from_this(), RtcStreamState::Failed);
        }
    }

    void RtcStream::OnTrack(
            rtc::scoped_refptr<webrtc::RtpTransceiverInterface> receiver) {
        _sigAddNewTrack(shared_from_this(), receiver);
    }

}
