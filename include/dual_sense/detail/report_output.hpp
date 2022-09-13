#ifndef DUAL_SENSE_REPORT_OUTPUT_HPP
#define DUAL_SENSE_REPORT_OUTPUT_HPP

#include <cstdint>

	namespace dual_sense::detail
	{
	#pragma pack(push, 1)

		struct Rumble
		{
			uint8_t right;
			uint8_t left;
		};

		struct SoundVolume
		{
			uint8_t headphones;
			uint8_t speaker;
			uint8_t mic;
		};

		struct SoundSettings
		{
			uint8_t microphone_selection: 2;

			uint8_t echo_cancel: 1;
			uint8_t noise_cancel: 1;

			uint8_t output_audio_path: 2;
			uint8_t input_audio_path: 2;
		};

		struct PowerSaveMute
		{
			uint8_t mute_light_mode;
			//power save

			uint8_t touchpad_power_save: 1;
			uint8_t motion_power_save: 1;
			uint8_t haptic_power_save: 1;
			uint8_t audio_power_save: 1;

			//mute

			uint8_t mic_mute: 1;
			uint8_t speaker_mute: 1;
			uint8_t headphone_mute: 1;
			uint8_t haptic_mute: 1;
		};

		struct TriggerForceFeedback
		{
			uint8_t right[11];
			uint8_t left[11];
		};

		struct MotorReduction
		{
			uint8_t trigger_power: 4;
			uint8_t rumble_power: 4;
		};

		struct AdditionalAudioSettings
		{
			uint8_t speaker_pre_gain: 3;
			uint8_t beamforming: 1;

			uint8_t gap_1: 4;
		};

		struct PlayerLed
		{
			uint8_t fade_animation;
			uint8_t brightness;

			uint8_t led_1: 1;
			uint8_t led_2: 1;
			uint8_t led_3: 1;
			uint8_t led_4: 1;
			uint8_t led_5: 1;
			uint8_t led_fade: 1;

			uint8_t gap_5: 2;
		};

		struct TouchpadLedColor
		{
			uint8_t red_led;
			uint8_t green_led;
			uint8_t blue_led;
		};

		struct SetStateReportCommon {
			//compatibility
			uint8_t rumble_emulation: 1;
			uint8_t use_haptics: 1;

			//enable section
			uint8_t enable_right_trigger_section: 1;
			uint8_t enable_left_trigger_section: 1;

			uint8_t enable_headphone_volume_section: 1;
			uint8_t enable_speaker_volume_section: 1;
			uint8_t enable_mic_volume_section: 1;
			uint8_t enable_audio_control_section: 1;
			uint8_t enable_mute_light_section: 1;
			uint8_t enable_audio_mute_section: 1;

			uint8_t enable_led_color_section: 1;
			uint8_t reset_lights : 1;
			uint8_t enable_player_indicators_section: 1;

			uint8_t enable_haptic_filter_section: 1;
			uint8_t enable_motor_section: 1;

			uint8_t enable_additional_audio_control_section: 1;

			Rumble rumble;

			SoundVolume volume;
			SoundSettings sound_settings;
			PowerSaveMute power_save_mute;

			TriggerForceFeedback trigger_force_feedback;

			uint32_t host_timestamp;

			MotorReduction motor_reduction;

			AdditionalAudioSettings additional_audio_settings;

			uint8_t enable_light_brightness_section: 1;
			uint8_t enable_color_light_fade_section: 1;

			uint8_t gap_2: 6;

			uint8_t haptic_low_pass_filter: 1;
			uint8_t gap_3: 7;
			uint8_t gap_4;

			PlayerLed player_led;

			TouchpadLedColor touchpad_led_color;
		};

		struct SetStateReportUSB
		{
			uint8_t report_id;
			SetStateReportCommon common;
		};

	#pragma pack(pop)

		static_assert(sizeof(SetStateReportCommon) == 47);
		static_assert(sizeof(SetStateReportUSB) == 48);
	}
#endif //DUAL_SENSE_REPORT_OUTPUT_HPP
