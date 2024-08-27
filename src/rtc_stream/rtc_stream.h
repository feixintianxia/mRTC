//
// Created by li on 8/15/24.
//

#pragma once

#include <memory>
#include "api/peer_connection_interface.h"

#include "api/common.h"

namespace mRTC {
    class RtcStream : public std::enable_shared_from_this<RtcStream>,
                      public webrtc::PeerConnectionObserver {
    public:
        using Ptr = std::shared_ptr<RtcStream>;

        RtcStream(const std::string uid, const std::string streamName);

        virtual ~RtcStream() {}

        bool init();

        void setRemoteOffer(const std::string &offer);

        void createAnswer();

        void setLocalAnswer(webrtc::SessionDescriptionInterface *desc);

    public:
        //信号
        sigslot::signal2<Ptr, const std::string &> _sigAnswerReady;
        sigslot::signal2<Ptr, RtcStreamState> _sigPcStateChange;
        sigslot::signal2<Ptr, rtc::scoped_refptr<webrtc::RtpTransceiverInterface> > _sigAddNewTrack;

    public:
        //webrtc::PeerConnectionObserver
        // Triggered when the SignalingState changed.
        void OnSignalingChange(
                webrtc::PeerConnectionInterface::SignalingState new_state);

        // Triggered when a remote peer opens a data channel.
        void OnDataChannel(
                rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);

        // A new ICE candidate has been gathered.
        void OnIceCandidate(const webrtc::IceCandidateInterface *candidate);

        // Called any time the IceGatheringState changes.
        void OnIceGatheringChange(
                webrtc::PeerConnectionInterface::IceGatheringState new_state);

        // Called any time the PeerConnectionState changes.
        void OnConnectionChange(
                webrtc::PeerConnectionInterface::PeerConnectionState new_state);

        // This is called when signaling indicates a transceiver will be receiving
        // media from the remote endpoint. This is fired during a call to
        // SetRemoteDescription. The receiving track can be accessed by:
        // `transceiver->receiver()->track()` and its associated streams by
        // `transceiver->receiver()->streams()`.
        // Note: This will only be called if Unified Plan semantics are specified.
        // This behavior is specified in section 2.2.8.2.5 of the "Set the
        // RTCSessionDescription" algorithm:
        // https://w3c.github.io/webrtc-pc/#set-description
        virtual void OnTrack(
                rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver);

    protected:
        std::string _uid;
        std::string _streamName;

        webrtc::PeerConnectionInterface::SignalingState _lastSignalingState =
                webrtc::PeerConnectionInterface::SignalingState::kClosed;
        webrtc::PeerConnectionInterface::IceGatheringState _lastGatheringState =
                webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringComplete;
        webrtc::PeerConnectionInterface::PeerConnectionState _lastPcState =
                webrtc::PeerConnectionInterface::PeerConnectionState::kClosed;

        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _pcFactory = nullptr;
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> _pc = nullptr;

    };

}


