#!/usr/bin/python
"""
  (C) Copyright 2020 Intel Corporation.

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
from __future__ import print_function

from command_utils_base import FormattedParameter
from command_utils_base import BasicParameter
from command_utils import ExecutableCommand
from job_manager_utils import Mpirun


class DcpCommand(ExecutableCommand):
    """Defines a object representing a dcp command."""

    def __init__(self, namespace, command):
        """Create a dcp Command object."""
        super(DcpCommand, self).__init__(namespace, command)

        # dcp options

        # IO buffer size in bytes (default 64MB)
        self.blocksize = FormattedParameter("--blocksize {}")
        # work size per task in bytes (default 64MB)
        self.chunksize = FormattedParameter("--chunksize {}")
        # DAOS source pool
        self.daos_src_pool = FormattedParameter("--daos-src-pool {}")
        # DAOS destination pool
        self.daos_dst_pool = FormattedParameter("--daos-dst-pool {}")
        # DAOS source container
        self.daos_src_cont = FormattedParameter("--daos-src-cont {}")
        # DAOS destination container
        self.daos_dst_cont = FormattedParameter("--daos-dst-cont {}")
        # DAOS prefix for unified namespace path
        self.daos_prefix = FormattedParameter("--daos-prefix {}")
        # read source list from file
        self.input_file = FormattedParameter("--input {}")
        # copy original files instead of links
        self.dereference = FormattedParameter("--dereference", False)
        # don't follow links in source
        self.no_dereference = FormattedParameter("--no-dereference", False)
        # preserve permissions, ownership, timestamps, extended attributes
        self.preserve = FormattedParameter("--preserve", False)
        # open files with O_DIRECT
        self.direct = FormattedParameter("--direct", False)
        # create sparse files when possible
        self.sparse = FormattedParameter("--sparse", False)
        # print progress every N seconds
        self.progress = FormattedParameter("--progress {}")
        # verbose output
        self.verbose = FormattedParameter("--verbose", False)
        # quiet output
        self.quiet = FormattedParameter("--quiet", False)
        # print help/usage
        self.print_usage = FormattedParameter("--help", False)
        # source path
        self.src_path = BasicParameter(None)
        # destination path
        self.dest_path = BasicParameter(None)

    def get_param_names(self):
        """Overriding the original get_param_names."""

        param_names = super(DcpCommand, self).get_param_names()

        # move key=dest_path to the end
        param_names.sort(key='dest_path'.__eq__)

        return param_names

    def set_dcp_params(self, src_pool=None, dst_pool=None, src_cont=None,
                       dst_cont=None, display=True):
        """Set the dcp params for the DAOS group, pool, and cont uuid.

        Args:
          src_pool(TestPool): source pool object
          dst_pool(TestPool): destination pool object
          src_cont(TestContainer): source container object
          dst_cont(TestContainer): destination container object
          display (bool, optional): print updated params. Defaults to True.
        """

        # set the obtained values
        if src_pool:
            self.daos_src_pool.update(src_pool.uuid,
                                      "daos_src_pool" if display else None)
        if dst_pool:
            self.daos_dst_pool.update(dst_pool.uuid,
                                      "daos_dst_pool" if display else None)

        if src_cont:
            self.daos_src_cont.update(src_cont.uuid,
                                      "daos_src_cont" if display else None)
        if dst_cont:
            self.daos_dst_cont.update(dst_cont.uuid,
                                      "daos_dst_cont" if display else None)

    def set_dcp_params_all(self,
                           src_pool=None, src_cont=None, src_path=None,
                           dst_pool=None, dst_cont=None, dst_path=None,
                           prefix=None, reset=False, display=True):
        """Set all common dcp params.

        Args:
            src_pool (str, optional): source pool uuid
            src_cont (str, optional): source container uuid
            src_path (str, optional): source path
            dst_pool (str, optional): destination pool uuid
            dst_cont (str, optional): destination container uuid
            dst_path (str, optional): destination path
            prefix (str, optional): prefix for uns path
            reset (bool, optional): reset all src or dst params before update.
                Defaults to False.
            display (bool, optional): print updated params. Defaults to True.

        """
        if reset:
            # Reset all params before updating
            self.daos_src_pool.update(None)
            self.daos_src_cont.update(None)
            self.src_path.update(None)
            self.daos_dst_pool.update(None)
            self.daos_dst_cont.update(None)
            self.dest_path.update(None)
            self.daos_prefix.update(None)

        if src_pool:
            self.daos_src_pool.update(src_pool,
                                      "daos_src_pool" if display else None)
        if src_cont:
            self.daos_src_cont.update(src_cont,
                                      "daos_src_cont" if display else None)
        if src_path:
            self.src_path.update(src_path,
                                 "src_path" if display else None)
        if dst_pool:
            self.daos_dst_pool.update(dst_pool,
                                      "daos_dst_pool" if display else None)
        if dst_cont:
            self.daos_dst_cont.update(dst_cont,
                                      "daos_dst_cont" if display else None)
        if dst_path:
            self.dest_path.update(dst_path,
                                 "dest_path" if display else None)
        if prefix:
            self.daos_prefix.update(prefix,
                                    "daos_prefix" if display else None) 

class Dcp(DcpCommand):
    """Class defining an object of type DcpCommand."""

    def __init__(self, hosts, timeout=30):
        """Create a dcp object."""
        super(Dcp, self).__init__(
            "/run/datamover/*", "dcp")

        # set params
        self.timeout = timeout
        self.hosts = hosts

    def run(self, tmp, processes):
        # pylint: disable=arguments-differ
        """Run the dcp command.

        Args:
            tmp (str): path for hostfiles
            processes: Number of processes for dcp command
        Raises:
            CommandFailure: In case dcp run command fails

        """
        self.log.info('Starting dcp')

        # Get job manager cmd
        mpirun = Mpirun(self, mpitype="mpich")
        mpirun.assign_hosts(self.hosts, tmp)
        mpirun.assign_processes(processes)
        mpirun.exit_status_exception = self.exit_status_exception

        # run dcp
        out = mpirun.run()

        return out


class FsCopyCommand(ExecutableCommand):
    """Defines a object representing a daos fs copy command."""

    def __init__(self, namespace, command):
        """Create a daos fs copy Command object."""
        super(FsCopyCommand, self).__init__(namespace, command)

        # daos fs copy options

        # TODO - comment
        self.src = FormattedParameter("--src {}")
        # TODO - comment
        self.dst = FormattedParameter("--dst {}")

    def set_fs_copy_params(self, src=None, dst=None,
                           reset=False, display=True):
        """Set the daos fs copy params.

        Args:
            src (str, optional): the src
            dst (str, optional): the dst
            reset (bool, optional): reset all src or dst params before update.
                Defaults to False.
            display (bool, optional): print updated params. Defaults to True.

        """
        if reset:
            # Reset all params before updating
            self.src.update(None)
            self.dst.update(None)

        if src:
            self.src.update(src, "src" if display else None)
        if dst:
            self.dst.update(dst, "dst" if display else None)


class FsCopy(FsCopyCommand):
    """Class defining an object of type FsCopyCommand."""

    def __init__(self, hosts, timeout=30):
        """Create a daos fs copy object."""
        super(FsCopy, self).__init__(
            "/run/datamover/*", "daos fs copy")

        # set params
        self.timeout = timeout
        self.hosts = hosts

    def run(self, tmp):
        # pylint: disable=arguments-differ
        """Run the daos fs copy command.

        Args:
            tmp (str): path for hostfiles
        Raises:
            CommandFailure: In case daos fs copy run command fails
        """
        self.log.info('Starting daos fs copy')

        # Get job manager cmd
        mpirun = Mpirun(self, mpitype="mpich")
        mpirun.assign_hosts(self.hosts, tmp)
        mpirun.assign_processes(1)
        mpirun.exit_status_exception = self.exit_status_exception

        # TODO don't use mpi for this; single process only
        # run daos fs copy
        out = mpirun.run()

        return out
