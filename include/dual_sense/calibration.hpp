#ifndef DUAL_SENSE_CALIBRATION_HPP
#define DUAL_SENSE_CALIBRATION_HPP

#include <cstdint>
#include <string>


namespace dual_sense
{
	/**
	 * @brief Calibration data for accelerometer & gyroscope
	 */
	struct Calibration
	{
		/**
		 * @brief Calibration data for gyroscope
		 */
		struct Gyroscope
		{
			/** Calibration factor numerator */
			int32_t factor_numerator;

			/**
			 * @name Pitch calibration data
			 */
			///@{
			/** Calibration factor denominator */
			int32_t pitch_factor_denominator;
			/** Calibration offset */
			int32_t pitch_offset;
			///@}

			/**
			 * @name Yaw calibration data
			 */
			///@{
			/** Calibration factor denominator */
			int32_t yaw_factor_denominator;
			/** Calibration offset */
			int32_t yaw_offset;
			///@}

			/**
			 * @name Roll calibration data
			 */
			///@{
			/** Calibration factor denominator */
			int32_t roll_factor_denominator;
			/** Calibration offset */
			int32_t roll_offset;
			///@}
		};

		/**
		 * @brief Calibration data for accelerometer
		 */
		struct Accelerometer
		{
			/** Calibration factor numerator */
			int32_t factor_numerator;

			/**
			 * @name X axis calibration data
			 */
			///@{
			/** Calibration factor denominator */
			int32_t x_factor_denominator;
			/** Calibration offset */
			int32_t x_offset;
			///@}

			/**
			 * @name Y axis calibration data
			 */
			///@{
			/** Calibration factor denominator */
			int32_t y_factor_denominator;
			/** Calibration offset */
			int32_t y_offset;
			///@}

			/**
			 * @name Z axis calibration data
			 */
			///@{
			/** Calibration factor denominator */
			int32_t z_factor_denominator;
			/** Calibration offset */
			int32_t z_offset;
			///@}
		};

		/**
		 * @brief Accelerometer reading resolution
		 */
		static constexpr uint16_t ACCELEROMETER_RESOLUTION = 8192;
		/**
		 * @brief Gyroscope reading resolution
		 */
		static constexpr uint16_t GYROSCOPE_RESOLUTION = 1024;

		Accelerometer accelerometer; /*!< Calibration data for accelerometer */
		Gyroscope gyroscope; /*!< Calibration data for gyroscope */
	};
}



#endif //DUAL_SENSE_CALIBRATION_HPP
