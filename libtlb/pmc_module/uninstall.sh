#!/bin/bash

[ "${UID}" -eq 0 ] || exec sudo bash "${0}" "${@}"

rmmod pmc_module
