#!/usr/bin/python
"""
(C) Copyright 2018-2020 Intel Corporation.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

GOVERNMENT LICENSE RIGHTS-OPEN SOURCE SOFTWARE
The Government's rights to use, modify, reproduce, release, perform, display,
or disclose this software are subject to the terms of the Apache License as
provided in Contract No. B609815.
Any reproduction of computer software, computer software documentation, or
portions thereof marked with this legend must also reproduce the markings.
"""
from command_utils_base import CommandFailure
from daos_utils import DaosCommand
from test_utils_pool import TestPool
from test_utils_container import TestContainer
from ior_test_base import IorTestBase
from data_mover_utils import Dcp, FsCopy
from os.path import join, sep
import uuid


class DataMoverTestBase(IorTestBase):
    # pylint: disable=too-many-ancestors
    """Base DataMover test class.

    Sample Use Case:
        set_ior_location_and_run("DAOS_UUID", "/testFile, pool1, cont1,
                                 flags="-w -K")
        set_src("DAOS_UUID", "/testFile", pool1, cont1)
        set_dst("POSIX", "/some/posix/path/testFile")
        set_ior_location_and_run("POSIX", "/some/posix/path/testFile",
                                 flags="-r -R")
    :avocado: recursive
    """

    # The valid parameter types for setting param locations.
    PARAM_TYPES = ("POSIX", "DAOS_UUID", "DAOS_UNS")

    # The valid datamover tools that can be used
    TOOLS = ("DCP", "FS_COPY")

    def __init__(self, *args, **kwargs):
        """Initialize a DataMoverTestBase object."""
        super(DataMoverTestBase, self).__init__(*args, **kwargs)
        self.tool = None
        self.dcp_cmd = None
        self.fs_copy_cmd = None
        self.datamover_processes = None
        self.pool = []
        self.containers = []
        self.uuids = []
        self.dfuse_hosts = None

    def setUp(self):
        """Set up each test case."""
        # Start the servers and agents
        super(DataMoverTestBase, self).setUp()

        self.dfuse_hosts = self.agent_managers[0].hosts

        # Get the parameters for DataMover
        self.dcp_cmd = Dcp(self.hostlist_clients)
        self.dcp_cmd.get_params(self)
        self.fs_copy_cmd = FsCopy(self.hostlist_clients)
        self.fs_copy_cmd.get_params(self)
        self.datamover_processes = self.params.get("np", "/run/datamover/processes/*")
        tool = self.params.get("tool", "/run/datamover/*")
        if tool:
            self.set_tool(tool)

        # List of test paths to create and remove
        self.posix_test_paths = []

        # List of daos test paths to keep traack of
        self.daos_test_paths = []

    def pre_tear_down(self):
        """Tear down steps to run before tearDown().

        Returns:
            list: a list of error strings to report at the end of tearDown().

        """
        error_list = []
        # Remove the created directories
        if self.posix_test_paths:
            command = "rm -rf {}".format(self.get_posix_test_path_string())
            try:
                self._execute_command(command)
            except CommandFailure as error:
                error_list.append(
                    "Error removing created directories: {}".format(error))
        return error_list

    def set_tool(self, tool):
        """Set the copy tool.

        Converts to upper-case and fails if the tool is not valid.

        Args:
            tool (str): the tool to use. Can be DCP or FS_COPY.

        """
        _tool = str(tool).upper()
        if _tool in self.TOOLS:
            self.tool = _tool
        else:
            self.fail("Invalid tool: {}".format(_tool))

    def get_posix_test_path_list(self):
        """Get a list of quoted posix test path strings.

        Returns:
            list: a list of quoted posix test path strings

        """
        return ["'{}'".format(item) for item in self.posix_test_paths]

    def get_posix_test_path_string(self):
        """Get a string of all of the quoted posix test path strings.

        Returns:
            str: a string of all of the quoted posix test path strings

        """
        return " ".join(self.get_posix_test_path_list())

    def new_posix_test_path(self, create=True, trailing_slash=False):
        """Create a new, unique posix path.

        Args:
            create (bool): Whether to create the directory.
                Defaults to True.
            trailing_slash (bool): Whether to append a trailing slash.
                Defaults to False.

        Returns:
            str: the posix path.

        """
        dir_name = "posix_test{}".format(len(self.posix_test_paths))
        path = join(self.workdir, dir_name)

        if trailing_slash:
            # Append a trailing slash
            path += sep

        # Add to the list of posix paths
        self.posix_test_paths.append(path)

        if create:
            # Create the directory
            cmd = "mkdir -p '{}'".format(path)
            self.execute_cmd(cmd)

        return path

    def new_daos_test_path(self, create=True, cont=None, parent="/"):
        """Create a new, unique daos container path.

        Args:
            create (bool, optional): Whether to create the directory.
                Defaults to True.
            cont (TestContainer, optional): The container to create the
                path within.
            parent (str, optional): The parent directory relative to the
                container root. Defaults to "/".

        Returns:
            str: the path relative to the root of the container.

        """
        dir_name = "daos_test{}".format(len(self.daos_test_paths))
        path = join(parent, dir_name)

        # Add to the list of daos paths
        self.daos_test_paths.append(path)

        if create:
            if not cont or not cont.path:
                self.fail("Container path required to create directory.")
            # Create the directory relative to the container path
            cmd = "mkdir -p '{}'".format(cont.path.value + path)
            self.execute_cmd(cmd)

        return path

    def validate_param_type(self, param_type):
        """Validates the param_type.

        It converts param_types to upper-case and handles shorthand types.

        Args:
            param_type (str): The param_type to be validated.

        Returns:
            str: A valid param_type

        """
        _type = str(param_type).upper()
        if _type == "DAOS":
            return "DAOS_UUID"
        if _type in self.PARAM_TYPES:
            return _type
        self.fail("Invalid param_type: {}".format(_type))

    @staticmethod
    def uuid_from_obj(obj):
        """Try to get uuid from an object.

        Args:
            obj (Object): The object possibly containing uuid.

        Returns:
            Object: obj.uuid if it exists; otherwise, obj
        """
        if hasattr(obj, "uuid"):
            return obj.uuid
        return obj

    def create_pool(self):
        """Create a TestPool object.

        Returns:
            TestPool: the created pool

        """
        # Get the pool params
        pool = TestPool(
            self.context, dmg_command=self.get_dmg_command())
        pool.get_params(self)

        # Create the pool
        pool.create()

        # Save the pool and uuid
        self.pool.append(pool)
        self.uuids.append(str(pool.uuid))

        return pool

    def create_cont(self, pool, use_dfuse_uns=False,
                    dfuse_uns_pool=None, dfuse_uns_cont=None):
        # pylint: disable=arguments-differ
        """Create a TestContainer object.

        Args:
            pool (TestPool): pool to create the container in.
            use_dfuse_uns (bool, optional): whether to create a
                UNS path in the dfuse mount.
                Default is False.
            dfuse_uns_pool (TestPool, optional): pool in the
                dfuse mount for which to create a UNS path.
                Default assumes dfuse is running for a specific pool.
            dfuse_uns_cont (TestContainer, optional): container in the
                dfuse mount for which to create a UNS path.
                Default assumes dfuse is running for a specific container.

        Returns:
            TestContainer: the container object

        Note about uns path:
            These are only created within a dfuse mount.
            The full UNS path will be created as:
            <dfuse.mount_dir>/[pool_uuid]/[cont_uuid]/<dir_name>
            dfuse_uns_pool and dfuse_uns_cont should only be supplied
            when dfuse was not started for a specific pool/container.

        """
        # Get container params
        container = TestContainer(
            pool, daos_command=DaosCommand(self.bin))
        container.get_params(self)

        if use_dfuse_uns:
            path = str(self.dfuse.mount_dir.value)
            if dfuse_uns_pool:
                path = join(path, dfuse_uns_pool.uuid)
            if dfuse_uns_cont:
                path = join(path, dfuse_uns_cont.uuid)
            path = join(path, "uns{}".format(str(len(self.containers))))
            container.path.update(path)

        # Create container
        container.create()

        # Save container and uuid
        self.containers.append(container)
        self.uuids.append(str(container.uuid))

        return container

    def gen_uuid(self):
        """Generate a unique uuid.

        Returns:
            str: a unique uuid

        """
        new_uuid = str(uuid.uuid4())
        while new_uuid in self.uuids:
            new_uuid = str(uuid.uuid4())
        return new_uuid

    def set_params(self, *args, **kwargs):
        """Set the params for self.tool."""
        if self.tool == "DCP":
            self.set_params_dcp(*args, **kwargs)
        elif self.tool == "FS_COPY":
            self.set_params_fs_copy(*args, **kwargs)
        else:
            self.fail("Invalid tool: {}".format(str(self.tool)))

    def set_params_dcp(self,
                       src_type=None, src_path=None,
                       src_pool=None, src_cont=None,
                       dst_type=None, dst_path=None,
                       dst_pool=None, dst_cont=None,
                       reset=True):
        """Set the params for dcp.

        When both src_type and dst_type are DAOS_UNS, a prefix will
        only work for either the src or the dst, but not both.

        Args:
            src_type (str): how to interpret the src params.
                Must be in PARAM_TYPES.
            src_path (str): posix-style source path.
                For containers, this is relative to the container root.
            src_pool (TestPool, optional): the source pool.
                Alternatively, this can the pool uuid.
            src_cont (TestContainer, optional): the source container.
                Alternatively, this can be the container uuid.
            dst_type (str): how to interpret the dst params.
                Must be in PARAM_TYPES.
            dst_path (str): posix-style destination path.
                For containers, this is relative to the container root.
            dst_pool (TestPool, optional): the destination pool.
                Alternatively, this can the pool uuid.
            dst_cont (TestContainer, optional): the destination container.
                Alternatively, this can be the container uuid.
            reset (bool, optional): reset all params before setting.
                Defaults to True.

        """
        if src_type is not None:
            src_type = self.validate_param_type(src_type)
        if dst_type is not None:
            dst_type = self.validate_param_type(dst_type)

        if reset:
             self.dcp_cmd.set_dcp_params_all(reset=True)

        # Set the source params
        if src_type == "POSIX":
            self.dcp_cmd.set_dcp_params_all(
                src_path=src_path)
        elif src_type == "DAOS_UUID":
            self.dcp_cmd.set_dcp_params_all(
                src_path=src_path,
                src_pool=self.uuid_from_obj(src_pool),
                src_cont=self.uuid_from_obj(src_cont))
        elif src_type == "DAOS_UNS":
            if src_cont:
                if src_path == "/":
                    self.dcp_cmd.set_dcp_params_all(
                        src_path=src_cont.path.value)
                else:
                    self.dcp_cmd.set_dcp_params_all(
                        prefix=src_cont.path.value,
                        src_path=src_cont.path.value + src_path)

        # Set the destination params
        if dst_type == "POSIX":
            self.dcp_cmd.set_dcp_params_all(
                dst_path=dst_path)
        elif dst_type == "DAOS_UUID":
            self.dcp_cmd.set_dcp_params_all(
                dst_path=dst_path,
                dst_pool=self.uuid_from_obj(dst_pool),
                dst_cont=self.uuid_from_obj(dst_cont))
        elif dst_type == "DAOS_UNS":
            if dst_cont:
                if dst_path == "/":
                    self.dcp_cmd.set_dcp_params_all(
                        dst_path=dst_cont.path.value)
                else:
                    self.dcp_cmd.set_dcp_params_all(
                        prefix=dst_cont.path.value,
                        dst_path=dst_cont.path.value + dst_path)

    def set_params_fs_copy(self,
                           src_type=None, src_path=None,
                           src_pool=None, src_cont=None,
                           dst_type=None, dst_path=None,
                           dst_pool=None, dst_cont=None,
                           reset=True):
        """Set the params for fs copy.

        daos fs copy does not support a "prefix" on UNS paths,
        so the param type for DAOS_UNS must have the path "/".

        Args:
            src_type (str): how to interpret the src params.
                Must be in PARAM_TYPES.
            src_path (str): posix-style source path.
                For containers, this is relative to the container root.
            src_pool (TestPool, optional): the source pool.
                Alternatively, this can the pool uuid.
            src_cont (TestContainer, optional): the source container.
                Alternatively, this can be the container uuid.
            dst_type (str): how to interpret the dst params.
                Must be in PARAM_TYPES.
            dst_path (str): posix-style destination path.
                For containers, this is relative to the container root.
            dst_pool (TestPool, optional): the destination pool.
                Alternatively, this can the pool uuid.
            dst_cont (TestContainer, optional): the destination container.
                Alternatively, this can be the container uuid.
            reset (bool, optional): reset all params before setting.
                Defaults to True.

        """
        if src_type is not None:
            src_type = self.validate_param_type(src_type)
        if dst_type is not None:
            dst_type = self.validate_param_type(dst_type)

        if reset:
            self.fs_copy_cmd.set_fs_copy_params(reset=True)

        # Set the source params
        if src_type == "POSIX":
            self.fs_copy_cmd.set_fs_copy_params(
                src="posix:{}".format(str(src_path)))
        elif src_type == "DAOS_UUID":
            pool_uuid = self.uuid_from_obj(src_pool)
            cont_uuid = self.uuid_from_obj(src_cont)
            path = str(src_path).lstrip("/")
            param = "daos:{}/{}/{}".format(pool_uuid, cont_uuid, path)
            self.fs_copy_cmd.set_fs_copy_params(
                src=param)
        elif src_type == "DAOS_UNS":
            path = ""
            if src_cont:
                if src_path == "/":
                    path = str(src_cont.path)
                else:
                    self.fail("daos fs copy does not support a prefix")
            self.fs_copy_cmd.set_fs_copy_params(
                src="daos:{}".format(path))

        # Set the destination params
        if dst_type == "POSIX":
            self.fs_copy_cmd.set_fs_copy_params(
                dst="posix:{}".format(str(dst_path)))
        elif dst_type == "DAOS_UUID":
            pool_uuid = self.uuid_from_obj(dst_pool)
            cont_uuid = self.uuid_from_obj(dst_cont)
            path = str(dst_path).lstrip("/")
            param = "daos:{}/{}/{}".format(pool_uuid, cont_uuid, path)
            self.fs_copy_cmd.set_fs_copy_params(
                dst=param)
        elif dst_type == "DAOS_UNS":
            path = ""
            if dst_cont:
                if dst_path == "/":
                    path = str(dst_cont.path)
                else:
                    self.fail("daos fs copy does not support a prefix")
            self.fs_copy_cmd.set_fs_copy_params(
                dst="daos:{}".format(path))

    def set_ior_location(self, param_type, path, pool=None, cont=None,
                         path_suffix=None, display=True):
        """Set the ior params based on the location.

        Args:
            param_type (str): how to interpret the location
            path (str): posix-style path.
                For containers, this is relative to the container root
            pool (TestPool, optional): the pool object
            cont (TestContainer, optional): the container object.
                Alternatively, this can be the container uuid
            path_suffix (str, optional): suffix to append to the path.
                E.g. path="/some/path", path_suffix="testFile"
            display (bool, optional): print updated params. Defaults to True.
        """
        param_type = self.validate_param_type(param_type)

        # Reset params
        self.ior_cmd.api.update(None)
        self.ior_cmd.test_file.update(None)
        self.ior_cmd.dfs_pool.update(None)
        self.ior_cmd.dfs_cont.update(None)
        self.ior_cmd.dfs_group.update(None)

        display_api = "api" if display else None
        display_test_file = "test_file" if display else None

        # Allow cont to be either the container or the uuid
        cont_uuid = cont.uuid if hasattr(cont, "uuid") else cont

        # Optionally append suffix
        if path_suffix:
            if path_suffix[0] == "/":
                path_suffix = path_suffix[1:]
            path = join(path, path_suffix)

        if param_type == "POSIX":
            self.ior_cmd.api.update("POSIX", display_api)
            self.ior_cmd.test_file.update(path, display_test_file)
        elif param_type in ("DAOS_UUID", "DAOS_UNS"):
            self.ior_cmd.api.update("DFS", display_api)
            self.ior_cmd.test_file.update(path, display_test_file)
            if pool and cont_uuid:
                self.ior_cmd.set_daos_params(self.server_group,
                                             pool, cont_uuid)
            elif pool:
                self.ior_cmd.set_daos_params(self.server_group,
                                             pool, None)

    def set_ior_location_and_run(self, param_type, path, pool=None, cont=None,
                                 path_suffix=None, flags=None, display=True):
        """Set the ior params based on the location and run ior with some flags.

        Args:
            param_type: see set_ior_location
            path: see set_ior location
            pool: see set_ior location
            cont: see set_ior location
            path_suffix: see set_ior location
            flags (str, optional): ior_cmd flags to set
            display (bool, optional): print updated params. Defaults to True.
        """
        self.set_ior_location(param_type, path, pool, cont, path_suffix)
        if flags:
            self.ior_cmd.flags.update(flags, "flags" if display else None)
        self.run_ior(self.get_ior_job_manager_command(), self.processes,
                     display_space=(True if pool else False), pool=pool)

    def run_datamover(self, test_desc=None,
                      expected_rc=0, expected_output=None,
                      processes=None):
        """Run the corresponding command specified by self.tool.

        Args:
            test_desc (str, optional): description to print before running
            expected_rc (int, optional): rc expected to be returned
            expected_output (list, optional): substrings expected to be output
            processes (int, optional): number of mpi processes.
                defaults to self.datamover_processes

        Returns:
            The result "run" object

        """
        # Default expected_output to empty list
        if not expected_output:
            expected_output = []

        # Convert singular value to list
        if not isinstance(expected_output, list):
            expected_output = [expected_output]

        if test_desc is not None:
            self.log.info("Running %s: %s", self.tool, test_desc)

        try:
            if self.tool == "DCP":
                if not processes:
                    processes = self.datamover_processes
                # If we expect an rc other than 0, don't fail
                self.dcp_cmd.exit_status_exception = (expected_rc == 0)
                result = self.dcp_cmd.run(self.workdir, processes)
            elif self.tool == "FS_COPY":
                # TODO don't use mpi here. Serial tool
                # If we expect an rc other than 0, don't fail
                self.dcp_cmd.exit_status_exception = (expected_rc == 0)
                result = self.fs_copy_cmd.run(self.workdir)
            else:
                self.fail("Invalid tool: {}".format(str(self.tool)))
        except CommandFailure as error:
            self.log.error("%s command failed: %s", str(self.tool), str(error))
            self.fail("Test was expected to pass but it failed: {}\n".format(
                test_desc))

        # Check the return code
        actual_rc = result.exit_status
        if actual_rc != expected_rc:
            self.fail("Expected (rc={}) but got (rc={}): {}\n".format(
                expected_rc, actual_rc, test_desc))

        # Check for expected output
        for s in expected_output:
            if s not in result.stdout:
                self.fail("Expected {}: {}".format(s, test_desc))

        return result
