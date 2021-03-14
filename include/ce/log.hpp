#pragma once

#include <string>

namespace ce
{
	/**
	 * Log application messages
	 */
	class log
	{
	public:
		/**
		 * Log critical message
		 */
		static void critical(const std::string &message);

		/**
		 * Log debug message
		 */
		static void debug(const std::string &message);

		/**
		 * Log error message
		 */
		static void error(const std::string &message);

		/**
		 * Log information message
		 */
		static void info(const std::string &message);

		/**
		 * Log verbose message
		 */
		static void verbose(const std::string &message);

		/**
		 * Log warning message
		 */
		static void warn(const std::string &message);

	private:
		log() = default;

		static void log_message(int priority, const std::string &message);
	};
}
