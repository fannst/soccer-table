#include "spfp_ext.hpp"

/// Gets the next LRAM home packet, returns nullptr if not exists.
spfp_lram_home_pkt_t *spfp_lram_home_next (spfp_lram_home_pkt_t *pkt) noexcept {
	if (!pkt->hnex) {
		return nullptr;
	}

	return reinterpret_cast<spfp_lram_home_pkt_t *> (pkt->next);
}

