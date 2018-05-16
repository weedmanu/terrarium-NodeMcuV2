.. :changelog:

History
-------

.. to_doc

---------------------
0.8.1.dev0
---------------------

    

---------------------
0.8.0 (2017-12-29)
---------------------

* Many new documentation enhancements (thanks to @rhpvorderman, and others)
* rename of shed-install to shed-tools and add a new --latest and --revision argument (thanks to @rhpvorderman)
* many fixes and new tests by (thanks to @mvdbeek)
* Parallelization of run-data-managers (thanks to @rhpvorderman)
* run-data-managers now uses more advanced templating for less repetitive input yamls (thanks to @rhpvorderman)
* run-data-managers now checks if a genome index is already present before running the data manager (thanks to @rhpvorderman)
* ephemeris will now use https by default instead of http (thanks to @bgruening)

---------------------
0.7.0 (2017-06-27)
---------------------

* Many new documentation enhancements (thanks to @rhpvorderman, @erasche, and others) -
  docs are now published to https://readthedocs.org/projects/ephemeris/.
* Fix problem with empty list options related to running data managers (thanks to @rhpvorderman).
* Enable data managers to run with API keys (thanks to @rhpvorderman).
* Add sleep command to wait for a Galaxy API to become available (thanks to @erasche).
* Preserve readable order of keys while processing tool lists (thanks to @drosofff).

---------------------
0.6.1 (2017-04-17)
---------------------

* Add Python 2 and 3 testing for all scripts against galaxy-docker-stable along with various
  refactoring to reduce code duplication and Python 3 fixes. `#36
  <https://github.com/galaxyproject/ephemeris/pull/36>`__

---------------------
0.6.0 (2017-04-10)
---------------------

* Add new connection options for setting up data libraries.

---------------------
0.5.1 (2017-04-07)
---------------------

* Fix new ``run-data-managers`` CLI entrypoint.

---------------------
0.5.0 (2017-04-06)
---------------------

* Add ``run-data-managers`` tool to trigger DM with multiple values and in order. `#30 <https://github.com/galaxyproject/ephemeris/pull/30>`_
* The workflow install tool now supports a directory of workflows. `#27 <https://github.com/galaxyproject/ephemeris/pull/27>`_
* enable global options in a tool yaml files, like `install_resolver_dependencies: true` `#26 <https://github.com/galaxyproject/ephemeris/pull/26>`_
* Mention mimum required galaxy versions. `#23 <https://github.com/galaxyproject/ephemeris/pull/23>`_ (thanks to @mvdbeek)
    

---------------------
0.4.0 (2016-09-07)
---------------------

* Be more generic in determining a server time-out (thanks to @afgane).
* Get tool list entrypoint and improvements (thanks to @mvdbeek).
* Rename ``tool_panel_section_name`` to ``tool_panel_section_label`` like
  ansible-galaxy-tools (thanks to @nturaga).
* Add missing file ``tool_list.yaml.sample`` (thanks to @nturaga).

---------------------
0.3.0 (2016-08-26)
---------------------

* More robust shed-install script, install dependencies by default, improve logging
  (thanks to @mvdbeek).

---------------------
0.2.0 (2016-08-15)
---------------------

* Add tool generate a tool list from a Galaxy workflow file
  (thanks to @drosofff).
* Fix various code quality issues including adding beta support
  for Python 3 (thanks in part to @mvdbeek).

---------------------
0.1.0 (2016-06-15)
---------------------

* Setup project, pull in scripts from `ansible-galaxy-tools
  <https://github.com/galaxyproject/ansible-galaxy-tools>`__
  and adapt them for usage as a library.

.. _bioblend: https://github.com/galaxyproject/bioblend/
.. _nose: https://nose.readthedocs.org/en/latest/
