#include "main.hpp"

#include "SPFPTermios.hpp"

int main(int argc, const char **argv) noexcept {
	//
	// Creates the session.
	//

	SPFP::TermiosSession session;

	const char *DEVICE_PATH = (argc > 1) ? argv[1] : "/dev/ttyUSB0";
	std::cout << "Using SPFP on device : " << DEVICE_PATH << std::endl;

	session.Connect (DEVICE_PATH, 115200);

	//
	// Writes the ready packet to the client.
	//

	{
		uint8_t *temp = new uint8_t[sizeof (SPFP::frame_t) + sizeof (SPFP::packet_t)];
		session.WriteReadyPacket (temp);
		delete temp;
	}

	//
	// Starts the event loop.
	//

	SPFP::frame_t *frame = reinterpret_cast<SPFP::frame_t *>(new uint8_t[512]);

	while (true) {
		session.TXPoll ();
		session.RXPoll ();

		// Checks if there are any packets available.
		if (session.Read (frame)) {
			const SPFP::packet_t *packet = reinterpret_cast<const SPFP::packet_t *>(frame->p);
			switch (packet->proto) {
				case static_cast<uint8_t>(SPFP::DefaultProto::Control):
					switch (packet->ptype) {
						case static_cast<uint8_t>(SPFP::CtrlProtoType::Ready_RX_TX):
							std::cout << "Client Ready for RX, TX" << std::endl;
							break;
					}
					break;
			}
		}

		// Forces context switch.
		std::this_thread::yield ();
	}

	return 0;
}
