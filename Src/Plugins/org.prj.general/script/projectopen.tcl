#################################################################
# projectopen.tcl
# Author     : blueant
# Version    : 4.0
# Date       : 2015-01-18
# Description: General project open
#################################################################

package require TclFace

set prj_vci_path [file dirname [info script]]

# refresh project docking pane
source "$prj_vci_path/projectpane.tcl"
source "$prj_vci_path/taskpane.tcl"

# show project information page
ShowHtmlPage "tcl:<file>$prj_vci_path/../html/information.tcl"
