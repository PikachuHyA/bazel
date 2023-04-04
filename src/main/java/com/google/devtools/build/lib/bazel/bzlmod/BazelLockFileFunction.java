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

import static java.nio.charset.StandardCharsets.UTF_8;

import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableMap;
import com.google.devtools.build.lib.actions.FileValue;
import com.google.devtools.build.lib.bazel.bzlmod.BazelModuleResolutionFunction.BazelModuleResolutionFunctionException;
import com.google.devtools.build.lib.cmdline.LabelConstants;
import com.google.devtools.build.lib.server.FailureDetails.ExternalDeps.Code;
import com.google.devtools.build.lib.vfs.FileSystemUtils;
import com.google.devtools.build.lib.vfs.Path;
import com.google.devtools.build.lib.vfs.Root;
import com.google.devtools.build.lib.vfs.RootedPath;
import com.google.devtools.build.skyframe.SkyFunction;
import com.google.devtools.build.skyframe.SkyFunctionException;
import com.google.devtools.build.skyframe.SkyFunctionException.Transience;
import com.google.devtools.build.skyframe.SkyKey;
import com.google.devtools.build.skyframe.SkyValue;
import com.google.gson.Gson;
import com.google.gson.JsonIOException;
import java.io.FileNotFoundException;
import java.io.IOException;
import javax.annotation.Nullable;

/** Reads the contents of the lock file into its value. */
public class BazelLockFileFunction implements SkyFunction {

  private final Path rootDirectory;

  private static final BzlmodFlagsAndEnvVars EMPTY_FLAGS =
      BzlmodFlagsAndEnvVars.create(
          ImmutableList.of(), ImmutableMap.of(), ImmutableList.of(), "", false, "", "");

  private static final BazelLockFileValue EMPTY_LOCKFILE =
      BazelLockFileValue.create(
          BazelLockFileValue.LOCK_FILE_VERSION, "", EMPTY_FLAGS, ImmutableMap.of());

  public BazelLockFileFunction(Path rootDirectory) {
    this.rootDirectory = rootDirectory;
  }

  @Override
  @Nullable
  public SkyValue compute(SkyKey skyKey, Environment env)
      throws SkyFunctionException, InterruptedException {
    RootedPath lockfilePath =
        RootedPath.toRootedPath(Root.fromPath(rootDirectory), LabelConstants.MODULE_LOCKFILE_NAME);

    // Add dependency on the lockfile to recognize changes to it
    if (env.getValue(FileValue.key(lockfilePath)) == null) {
      return null;
    }

    BazelLockFileValue bazelLockFileValue;
    Gson gson = GsonTypeAdapterUtil.getLockfileGsonWithTypeAdapters();
    try {
      String json = FileSystemUtils.readContent(lockfilePath.asPath(), UTF_8);
      bazelLockFileValue = gson.fromJson(json, BazelLockFileValue.class);
    } catch (FileNotFoundException e) {
      bazelLockFileValue = EMPTY_LOCKFILE;
    } catch (IOException ex) {
      throw new JsonIOException("Failed to read or parse module-lock file", ex);
    }
    return bazelLockFileValue;
  }

  /**
   * Updates the stored module in the lock file (ModuleHash, Flags & Dependency graph)
   *
   * @param hashedModule The hash of the current module file
   * @param resolvedDepGraph The resolved dependency graph from the module file
   */
  public static void updateLockedModule(
      String hashedModule,
      ImmutableMap<ModuleKey, Module> resolvedDepGraph,
      Path rootDirectory,
      BzlmodFlagsAndEnvVars flags)
      throws BazelModuleResolutionFunctionException {
    RootedPath lockfilePath =
        RootedPath.toRootedPath(Root.fromPath(rootDirectory), LabelConstants.MODULE_LOCKFILE_NAME);

    BazelLockFileValue value =
        BazelLockFileValue.create(
            BazelLockFileValue.LOCK_FILE_VERSION, hashedModule, flags, resolvedDepGraph);
    Gson gson = GsonTypeAdapterUtil.getLockfileGsonWithTypeAdapters();
    try {
      FileSystemUtils.writeContent(lockfilePath.asPath(), UTF_8, gson.toJson(value));
    } catch (IOException e) {
      throw new BazelModuleResolutionFunctionException(
          ExternalDepsException.withCauseAndMessage(
              Code.BAD_MODULE, e, "Unable to update module-lock file"),
          Transience.PERSISTENT);
    }
  }
}
