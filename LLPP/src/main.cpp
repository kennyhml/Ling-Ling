
#include <dpp/dpp.h>

#include "common/webhook.h"

int main()
{

	llpp::common::discord::InitWebhook("https://dcord.com/api/webhooks/1178195420002922586/O2vfiRTSxDMsWJm2phzYuiGX8M16_2_yzs3OiFelmF6sK3d17yYR53byu7fpKaHM-Y3S");

	llpp::common::discord::Send("Helo man");

	return 0;
}