/**
 * @file
 */
#pragma once

/** @namespace */
namespace webby {
  /** @namespace */
  namespace strategy {
    /**
     * @brief Implements a single-client strategy.
     *
     * This is the simplest of all server strategies: A single process with only the main thread.
     * Only one client request can be serviced at a time. The operating system queues incoming
     * requests and releases them to the server when the current request has been handled.
     */
    class single {
      public:
        /**
         * @brief Default constructor
         */
        single() {
        }

        /**
         * @brief Destructor
         */
        ~single() {
        }

      private:
        // There should be only one active strategy. Removing the copy and move constructors and
        // the assignment operator help to enforce this.
        single(const single&) = delete;
        single(const single&&) = delete;
        void operator=(const single&) = delete;
    };
  }
}
