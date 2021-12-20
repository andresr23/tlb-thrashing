# Effective TLB Trashing
This repository contains all the source code used for the paper "**Effective TLB Trashing**", which has been accepted by The 37th ACM/SIGAPP Symposium On Applied Computing (SAC'22).

## Directories
-`libtlb`: Library that contains all the code used to properly configure and spawn a TLB indexing space, as well as auxiliary utilities such as data structures and functions to manipulate PTE sets. Inside there is also a `pmc_module` sub-directory that contains the implementation of a kernel module to configure Performance Monitoring Counters (PMCs), which are used by all the experiments described in the paper. Each experiment runs via a script that checks if the kernel module has been installed, and if not, then the script builds the module and installs it.
-`basic-tlb-eviction`: Experiment that evaluates the basic TLB eviction approach that consists of saturating the TLB with numerous PTEs.
-`inclusivity`: Test to verify that the L2-sTLB is indeed exclusive with respect to the L1-dTLB. That is, a PTE in the L2-sTLB must not be in the L1-dTLB.
-`inv-7`: Small experiment to showcase how it is possible to modify the `xor_7` hash equation to find PTEs that are congruent in specific L2-sTLB sets.
-`l1-dtlb`: Experiments to compute the L1-dTLB Set Eviction and Flooding rates, as presented in the paper.
-`l2-stlb`: Experiment to compute the L2-sTLB Set Eviction rate.
-`trel`: Small test to verify the composition of Target-Relative PTE sets.
-`xor-n`: Program that finds the solutions of an `xor_n` hash.

## Building the Project
Building each experiment is as easy as running `make` on each experiment sub-directory.

## Usage
Each experiment sub-directory includes a `start.sh` script that first checks if PMCs have been configured via the provided kernel module. The script then uses `gnuplot` to generate plots like the ones presented in the paper.

## Disclaimer
Using PMCs requires super-user privileges to install the provided kernel module. The module does nothing more than change some Model-Specific Register (MSR) values upon installation and, when uninstalled, the module reverts all the changes it has made.
Additionally, the results presented in the paper were obtained from running the experiments on a mostly-idle system.
