simquad -- generate uniform simple quadrangulations

* 
* 1. Introduction
* 

This project implements a random generator for uniform simple quadrangulations of the 2-sphere with a given number of quadrangles. It is based on the bijection between simple quadrangulations and 1-blossoming trees by Fusy (2010, PhD thesis at École Polytechnique), and a generating procedure for random blossoming trees by Addario-Berry and Albenque (2017, The Annals of Probability).


*
* 2. Usage
*

Usage: simquad [OPTION...] 
simquad -- generate uniform simple quadrangulations

  -N, --num=NUM              Simulate NUM many samples.
  -r, --randgen=RANDGEN      Use the pseudo random generator RANDGEN. Available
                             options are taus2, gfsr4, mt19937, ranlux,
                             ranlxs0, ranlxs1, ranlxs2, ranlxd1, ranlxd2, mrg,
                             cmrg, ranlux389. The default is taus2.
  -s, --size=SIZE            Generate a uniform planted simple quadrangulation
                             with s >= 6 faces,  s + 2 vertices, and 2s edges.
                             Has to be greater than or equal to six.
  -S, --seed=SEED            Specify the seed of the random generator in the
                             first thread. Thread number k will receive SEED +
                             k - 1 as seed. The default is to set SEED to the
                             systems timestamp (in seconds).
  -t, --threads=THREADS      Distribute the workload on THREADS many threads.
                             The default value is the number of CPU cores.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.


Each generated map is represented by a list of half edge data structures in the format {next, previous, opposite}. The total output is a list of lists, each element corresponding to a map.


*
* 3. Example
*

simquad -s 10
{{{32,34,2},{3,9,1},{4,2,38},{9,3,5},{6,8,4},{7,5,37},{8,6,22},{5,7,15},{2,4,10},{11,40,9},{12,10,14},{40,11,13},{14,16,12},{15,13,11},{16,14,8},{13,15,17},{18,24,16},{19,17,21},{24,18,20},{21,23,19},{22,20,18},{23,21,7},{20,22,30},{17,19,25},{26,39,24},{27,25,29},{39,26,28},{29,31,27},{30,28,26},{31,29,23},{28,30,32},{33,1,31},{34,32,36},{1,33,35},{36,38,34},{37,35,33},{38,36,6},{35,37,3},{25,27,40},{10,12,39}}}


*
* 4. LICENCE
*

Copyright (C) 2021 Benedikt Stufler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
