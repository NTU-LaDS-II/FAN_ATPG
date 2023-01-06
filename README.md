# LaDS   &emsp; &emsp; &emsp;   ATPG v2023     &emsp; &emsp; &emsp; JAN 2023

## CONTENTS

> **I.    Introduction**<br />
> **II.   Directory Structure**<br />
> **III.  Usage**<br />
> **IV.   Revision History**<br />
> **V.    Authors**<br />


## I. INTRODUCTION

**ATPG and fault simulation tool based on the FAN algorithm.** <br />
**Has Fault Collapsing, STC, and DTC with multiple fault orders.** <br />

## II. DIRECTORY STRUCTURE

> **Main files :** pkg/core<br />
> **Fan's commands files :** pkg/fan<br />
> **Interface files :** pkg/interface<br />
> **Commands, options and arguments manager files :** pkg/common<br />
> **Circuit files :** netlist<br />
> **Pattern files :** pat<br />
> **Script files :** script<br />


## III. USAGE

> **Compile:**<br />
> &emsp;	make <br />
<br />

> **Run ATPG:**<br />
> &emsp;	./bin/opt/fan <br />
> &emsp;	source script/atpg.script <br />
<br />

> **Run fault simulation:**<br />
> &emsp;	./bin/opt/fan <br />
> &emsp;	source script/fsim.script <br />


## IV. REVISION HISTORY

**v2013 - Initial release.** <br />
**v2023 - Code refactor.**


## V. AUTHORS

Hsu Po-Ya, Chen Yu-Chen <br />
Han Cheng-You, Chen Ching-Yu, Chiang Kuan-Ying, Wang Ying-Hsu, Chen Po-Hao, Lin Kuo-Yu, Pan Chun-Han, Li Chia-An, Tsai Chia-Ling, Hsu Ling-Yun <br />
Wang Wei-Shen, Chang Hsin-Tzu, Pan Yu-Hung <br />


***Copyright (c) 2013 LaDS. All Rights Reserved.***