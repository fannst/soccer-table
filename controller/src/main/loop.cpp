#include "main.hpp"


/// Gets called over and over again, used for process main loop.
void Main::Loop (void) noexcept {
  SPFP::frame_t *frame = reinterpret_cast<SPFP::frame_t *>(m_SPFPUsartSession.READ_BUFF);
  if (m_SPFPUsartSession.Read (frame)) {
    const SPFP::packet_t *packet = reinterpret_cast<const SPFP::packet_t *>(frame->p);
    switch (packet->proto) {
      case static_cast<uint8_t>(SPFP::DefaultProto::Control):
        HandleControlPacket (frame);
    }
  }

	Watchdog::Kick ();
}

/// Gets called over and over again, used for polling to replace interrupts.
void Main::Poll (void) noexcept {
  m_SPFPUsartSession.RXPoll ();     // Performs the RX Polling.
  m_SPFPUsartSession.TXPoll ();     // Performs the TX polling.
}

/// Handles control protocol packet.
void Main::HandleControlPacket (const SPFP::frame_t *frame) noexcept {
  const SPFP::packet_t *packet = reinterpret_cast<const SPFP::packet_t *>(frame->p);

  switch (packet->ptype) {
    case static_cast<uint8_t>(SPFP::CtrlProtoType::Ready_RX_TX):
      m_Buzzer.Buzz(200, 600);

      m_SPFPUsartSession.WriteReadyPacket (m_SPFPUsartSession.WRITE_BUFF);
      break;
  }
}