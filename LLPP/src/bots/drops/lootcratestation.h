#pragma once
#include "../../core/basestation.h"
#include "config.h"
#include <asapp/structures/cavelootcrate.h>
#include <asapp/structures/simplebed.h>
#include <asapp/structures/teleporter.h>

namespace llpp::bots::drops
{
	class LootCrateStation final : public core::BaseStation
	{
	public:
		explicit LootCrateStation(const std::string& t_name,
			CrateManagerConfig& t_config,
			asa::structures::CaveLootCrate t_crate);

		core::StationResult complete() override;

		void set_default_dst(const bool is_default)
		{
			is_default_dst_ = is_default;
		}
		void set_cooldown()
		{
			last_completed_ = std::chrono::system_clock::now();
		}
		void set_rendered(const bool rendered) { is_rendered_ = rendered; }

		[[nodiscard]] int get_times_looted() const { return times_looted_; }
		[[nodiscard]] int get_vault_slots() const { return vault_slots_; }

		const asa::structures::Container& get_vault() const { return vault_; }

	private:
		CrateManagerConfig& config_;

		std::map<std::string, bool> cherry_pick_items() const;
		void loot_crate(cv::Mat& screenshot_out,
			std::map<std::string, bool>& cherry_picked_out);
		void go_to();
		bool has_buff_wait_expired() const;
		bool should_reroll() const;
		int times_looted_ = 0;
		int vault_slots_ = 0;

		bool is_crate_up_ = false;
		bool is_rendered_ = false;
		bool is_default_dst_ = false;

		std::chrono::system_clock::time_point last_discovered_up_;
		std::chrono::minutes max_buff_wait_time_ = std::chrono::minutes(15);

		asa::structures::CaveLootCrate crate_;
		asa::structures::Teleporter teleporter_;
		asa::structures::SimpleBed bed_;
		asa::structures::Container vault_;
	};
}
