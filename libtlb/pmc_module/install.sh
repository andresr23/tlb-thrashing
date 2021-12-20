#!/bin/bash

[ "${UID}" -eq 0 ] || exec sudo bash "${0}" "${@}"

if [ "${1}" == "amd" ]; then
  echo 2 > /sys/devices/cpu/rdpmc
fi

insmod pmc_module.ko
