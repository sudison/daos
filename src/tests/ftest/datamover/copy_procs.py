#!/usr/bin/python
'''
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
'''
from data_mover_test_base import DataMoverTestBase
from os.path import join


class CopyProcsTest(DataMoverTestBase):
    # pylint: disable=too-many-ancestors
    """Test class for POSIX DataMover multiple processes.

    Test Class Description:
        Tests multi-process (rank) copying of the POSIX DataMover utility.
        Tests the following cases:
            Copying with varying numbers of processes (ranks).

    :avocado: recursive
    """

    def setUp(self):
        """Set up each test case."""
        # Start the servers and agents
        super(CopyProcsTest, self).setUp()

        # Get the parameters
        self.test_file = self.ior_cmd.test_file.value
        self.flags_write = self.params.get(
            "flags_write", "/run/ior/copy_procs/*")
        self.flags_read = self.params.get(
            "flags_read", "/run/ior/copy_procs/*")

    def test_copy_procs(self):
        """
        Test Description:
            Tests POSIX copy with multiple processes.
            DAOS-5659: Verify multi-process (rank) copying.
        Use Cases:
            Create pool.
            Crate POSIX container1 and container2 in pool.
            Create a single 100M file in container1 using ior.
        :avocado: tags=all,pr,datamover,dcp
        :avocado: tags=copy_procs
        """
        # Create pool and containers
        pool1 = self.create_pool()
        container1 = self.create_cont(pool1)
        container2 = self.create_cont(pool1)

        # Get the varying number of processes
        procs_list = self.params.get(
            "processes", "/run/datamover/copy_procs/*")

        # Generate test file paths
        src_daos = join("/", self.test_file)
        dst_daos = join("/", self.test_file)
        src_posix = join(self.new_posix_test_path(), self.test_file)
        dst_posix = join(self.new_posix_test_path(), self.test_file)

        # Create the test files
        self.set_ior_location_and_run("DAOS", src_daos,
                                      pool1, container1,
                                      flags=self.flags_write)
        self.set_ior_location_and_run("POSIX", src_posix,
                                      flags=self.flags_write)

        # DAOS -> POSIX
        # Run with varying number of processes
        self.set_params(
            "DAOS_UUID", src_daos, pool1, container1,
            "POSIX", dst_posix)
        for num_procs in procs_list:
            test_desc = "copy_procs (DAOS->POSIX with {} procs)".format(
                num_procs)
            self.run_datamover(
                test_desc=test_desc,
                processes=num_procs)
            self.set_ior_location_and_run("POSIX", dst_posix,
                                          flags=self.flags_read)

        # POSIX -> DAOS
        # Run with varying number of processes
        self.set_params(
            "POSIX", src_posix, None, None,
            "DAOS_UUID", dst_daos, pool1, container2)
        for num_procs in procs_list:
            test_desc = "copy_procs (POSIX->DAOS with {} procs)".format(
                num_procs)
            self.run_datamover(
                test_desc=test_desc,
                processes=num_procs)
            self.set_ior_location_and_run("DAOS_UUID", dst_daos,
                                          pool1, container2,
                                          flags=self.flags_read)
