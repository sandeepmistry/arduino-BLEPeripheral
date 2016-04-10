#/bin/bash

function set_version () {
  local VERSION="${1}"
  local FILE="${2}"
  echo "Updating '${FILE}' to '${VERSION}'."
  sed -i '' -e"s|\(version.*\)[0-9]\.[0-9]\.[0-9]|\1${VERSION}|g" "${FILE}"
}

function man () {
  local _B=$(tput bold)
  local B_=$(tput sgr0)
  local _U=$(tput smul)
  local U_=$(tput rmul)

  echo "${_B}NAME${B_}"
  echo "     ${_B}${0##*/}${B_} -- set library version"
  echo
  echo "${_B}SYNOPSIS${B_}"
  echo "     ${_B}${0##*/}${B_} ${_U}version${U_}"
  echo
  echo "${_B}DESCRIPTION${B_}"
  echo "     Utility for updating BLEPeripheral library to specified version."
  echo
  echo "${_B}EXAMPLE${B_}"
  echo "     ${_B}${0##*/}${B_} 4.1.22"
}



if [ -z "${1}" ]; then
  (>&2 echo "Error: Mandatory version argument is missing!")
  man
  exit -1
fi

VERSION="${1}"

set_version ${VERSION} "library.properties"
set_version ${VERSION} "library.json"
