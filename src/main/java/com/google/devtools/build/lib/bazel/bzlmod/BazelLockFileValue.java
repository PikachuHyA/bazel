// Copyright 2023 The Bazel Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

package com.google.devtools.build.lib.bazel.bzlmod;

import com.google.auto.value.AutoValue;
import com.google.common.collect.ImmutableMap;
import com.google.devtools.build.lib.skyframe.SkyFunctions;
import com.google.devtools.build.lib.skyframe.serialization.autocodec.SerializationConstant;
import com.google.devtools.build.skyframe.SkyKey;
import com.google.devtools.build.skyframe.SkyValue;
import com.ryanharter.auto.value.gson.GenerateTypeAdapter;
import java.util.Optional;

/**
 * The result of reading the lockfile. Contains the lockfile version, module hash, definitions of
 * module repositories, post-resolution dependency graph and module extensions data (ID, hash,
 * definition, usages)
 */
@AutoValue
@GenerateTypeAdapter
public abstract class BazelLockFileValue implements SkyValue {

  public static final int LOCK_FILE_VERSION = 1;

  @SerializationConstant public static final SkyKey KEY = () -> SkyFunctions.BAZEL_LOCK_FILE;

  public static BazelLockFileValue create(
      int lockFileVersion,
      String moduleFileHash,
      BzlmodFlagsAndEnvVars flags,
      ImmutableMap<ModuleKey, Module> moduleDepGraph) {
    return new AutoValue_BazelLockFileValue(lockFileVersion, moduleFileHash, flags, moduleDepGraph);
  }

  /** Current version of the lock file */
  public abstract int getLockFileVersion();

  /** Hash of the Module file */
  public abstract String getModuleFileHash();

  /** Command line flags and environment variables that can affect the resolution */
  public abstract BzlmodFlagsAndEnvVars getFlags();

  /** The post-selection dep graph retrieved from the lock file. */
  public abstract ImmutableMap<ModuleKey, Module> getModuleDepGraph();

  public Optional<RepoSpec> getRepoSpecOfModule(String repoName) {
    Optional<Module> repoModule =
        getModuleDepGraph().values().stream()
            .filter(module -> module.getRepoName().equals(repoName))
            .findAny();
    return Optional.ofNullable(repoModule.isPresent() ? repoModule.get().getRepoSpec() : null);
  }
}
