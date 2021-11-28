// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Scoped handler when new operator fails to allocate memory.

#include "scoped_new_handler.h"

#ifdef WB_OS_WIN
#include <limits>
#include <random>

#include "base/tests/g3log_death_utils_tests.h"
#endif
#include <vector>

#include "base/deps/googletest/gtest/gtest.h"

// NOLINTNEXTLINE(cert-err58-cpp, cppcoreguidelines-avoid-non-const-global-variables, cppcoreguidelines-owning-memory)
GTEST_TEST(ScopedNewHandlerTest, SetNewFailureHandlerInScope) {
  using namespace wb::base;

  EXPECT_NE(&DefaultNewFailureHandler, std::get_new_handler());

  {
    const ScopedNewHandler scoped_new_handler{DefaultNewFailureHandler};

    EXPECT_EQ(&DefaultNewFailureHandler, std::get_new_handler());
  }

  EXPECT_NE(&DefaultNewFailureHandler, std::get_new_handler());
}

// On POSIX just receive SIGKILL on OOM and we have no way to handle it.
#if defined(GTEST_HAS_DEATH_TEST) && defined(WB_OS_WIN)
// NOLINTNEXTLINE(cert-err58-cpp, cppcoreguidelines-avoid-non-const-global-variables, cppcoreguidelines-owning-memory)
GTEST_TEST(ScopedNewHandlerDeathTest, OutOfMemoryTriggersNewFailureHandler) {
  GTEST_FLAG_SET(death_test_style, "threadsafe");

  const wb::base::ScopedNewHandler scoped_new_handler{
      wb::base::DefaultNewFailureHandler};

  const auto triggerOom = []() noexcept {
    std::random_device random_device;
    std::mt19937 generator{random_device()};
    std::uniform_int_distribution<> distribution{1, 255};

    constexpr size_t doubled_total_ram_bytes{static_cast<size_t>(32) * 1028 *
                                             1028 * 1028};
    std::cerr << "Total RAM size: " << doubled_total_ram_bytes / 1024 / 1024
              << "MiBs.\n";

    constexpr size_t kBlockAllocSize{std::numeric_limits<unsigned>::max() >>
                                     1U};
    std::cerr << "Allocate RAM in blocks of " << kBlockAllocSize / 1024 / 1024
              << " MiBs\n";

    std::vector<int *> memory;

    size_t allocated_bytes{0};
    while (allocated_bytes < doubled_total_ram_bytes) {
      auto *block = new int[kBlockAllocSize];

      constexpr size_t kStepSize{65536}, kFillAreaSize{16};
      for (size_t i{0}; i < kBlockAllocSize - kFillAreaSize; i += kStepSize) {
        memset(&block[i], distribution(generator), kFillAreaSize);
      }

      memory.push_back(block);

      for (const auto *const it : memory) {
        for (size_t i{0}; i < kBlockAllocSize - kFillAreaSize; i += kStepSize) {
          // NOLINTNEXTLINE(bugprone-lambda-function-name): Defined in deps.
          G3CHECK((static_cast<unsigned>(it[i]) >> 24) >= 1U);
        }
      }

      allocated_bytes += kBlockAllocSize;

      std::cerr << "Allocated RAM " << allocated_bytes / 1024 / 1024
                << "MiB.\n";
    }

    // Memory is freed automatically as dead test is running in a distinct
    // process.  Makes test finish faster.
  };

  const auto test_result =
      wb::base::tests_internal::MakeG3LogCheckFailureDeathTestResult(
          "Failed to allocate memory bytes via new.  Please, ensure you "
          "have enough RAM to run the app.  Stopping the app.");

  EXPECT_EXIT(triggerOom(), test_result.exit_predicate, test_result.message);
}
#endif
