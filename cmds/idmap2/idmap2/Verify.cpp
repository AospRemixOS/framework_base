/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "idmap2/CommandLineOptions.h"
#include "idmap2/Idmap.h"
#include "idmap2/Result.h"
#include "idmap2/SysTrace.h"

using android::idmap2::CommandLineOptions;
using android::idmap2::Error;
using android::idmap2::IdmapHeader;
using android::idmap2::Result;
using android::idmap2::Unit;

Result<Unit> Verify(const std::vector<std::string>& args) {
  SYSTRACE << "Verify " << args;
  std::string idmap_path;

  const CommandLineOptions opts =
      CommandLineOptions("idmap2 verify")
          .MandatoryOption("--idmap-path", "input: path to idmap file to verify", &idmap_path);

  const auto opts_ok = opts.Parse(args);
  if (!opts_ok) {
    return opts_ok.GetError();
  }

  std::ifstream fin(idmap_path);
  const std::unique_ptr<const IdmapHeader> header = IdmapHeader::FromBinaryStream(fin);
  fin.close();
  if (!header) {
    return Error("failed to parse idmap header");
  }

  const auto header_ok = header->IsUpToDate();
  if (!header_ok) {
    return Error(header_ok.GetError(), "idmap not up to date");
  }

  return Unit{};
}
