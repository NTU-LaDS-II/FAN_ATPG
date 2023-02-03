# Script

<small><i>Wang Wei-Shen Last modified in Jan 2023</i></small>

This folder contains some scripts the users might want to use for convenience.

    .
    |--. autoRunScripts/          # Contains scripts for automating task across all circuits.
    |  |
    |  |-- averageAllFC.sh        # Calculate and log the average Fault Coverage of all the circuits that have report in the rpt/ folder.
    |  |
    |  |-- averageAllTL.sh        # Calculate and log the average Test Length of all the circuits that have report in the rpt/ folder.
    |  |
    |  `-- runFAN.sh              # Execute all the script in a  user specified folder, e.g. fanScripts/ or fanScriptsNoDTC/
    |
    |-- fanScripts/               # Contains scripts for ATPG and Fault Simulation with Dynamic Test Compression turned ON for ATPG.
    |
    `-- fanScriptsNoDTC/          # Contains scripts for ATPG and Fault Simulation with Dynamic Test Compression turned OFF for ATPG.

## Contact

Wang Wei-Shen - b08901051@ntu.edu.tw