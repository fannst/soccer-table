#include "main.hpp"

#include "spfp.h"
#include "helpers/linux.h"
#include "spfp_ext.hpp"

void send_beep_packet (spfp_linux_session_t *session) noexcept {
	// Calculates the resulting packet size.
	constexpr uint16_t size = sizeof (spfp_packet_t)
		+ sizeof (spfp_class_pkt_t)
		+ sizeof (spfp_system_pkt_t);

	// Sets the packet contents.

	spfp_packet_t *packet = reinterpret_cast<spfp_packet_t *>(malloc (size));
	
	spfp_class_pkt_t *class_pkt = reinterpret_cast<spfp_class_pkt_t *>(packet->p);
	class_pkt->c = SPFP_CLASS_SYSTEM;

	spfp_system_pkt_t *system_pkt = reinterpret_cast<spfp_system_pkt_t *>(class_pkt->p);
	system_pkt->op = SPFP_CONTROL_OP_BEEP;

	packet->len = size;
	packet->cs = spfp_calc_checksum (packet);

	// Writes the packet.
	spfp_linux_session_write (session, packet);

	free (packet);
}

int main(int argc, const char **argv) noexcept {
	std::cout << "Creating SPFP Linux session ..." << std::endl;
	spfp_linux_session_t *session = spfp_linux_session_create("/dev/ttyUSB0", 2048, 9600);
	std::cout << "SPFP Linux session created!" << std::endl;

	send_beep_packet (session);

	return 0;
}
