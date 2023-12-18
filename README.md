# DeBruijn Sequence Complexity Distribution Over Prime Fields
[![License](https://img.shields.io/badge/license-Apache_2.0-green)](https://github.com/joannj35/debruijn_sequence_complexity/blob/main/LICENSE)

## Overview
In this project we continue the study of the linear complexity of de Bruijn sequences over finite fields. Our aim is to establish the integer values of
linear complexity for which there exist de Bruijn sequences of given span over finite fields. **This repository includes the code developed for calculating these complexity distributions.**

## Research Domains
Our study is divided into two distinct research domains:
  
### Binary Fields
This section examines de Bruijn sequences within binary fields (fields of prime 2), focusing on sequences with spans of 6 and 7. The code is found under `binary-fields` branch where the computation of the complexities utilizes *multithreading* and *OpenMP* to efficiently handle complex calculations and data processing.

### Non-Binary Fields
This section examines de Bruijn sequences within non-binary fields (fields of prime 3,5,7), particularly focusing on sequences with span 2. The code is found under `non-binary-fields` branch.

## Additional Tools and Visualization
For a more enhanced user experience, we have developed a (humble) GUI tool, housed in a separate repository. This GUI effectively visualizes the result data, providing a more intuitive understanding of the complexity distributions. For access to the GUI and further instructions, please refer to [link to GUI repository]().

## Using the Code
Each branch of this repository contains specific instructions and scripts for the respective domain. Please refer to the README files within each branch for detailed usage guidelines.

## Contributing
Contributions to this project are highly appreciated. If you have suggestions or improvements, please fork the appropriate branch of the repository, make your changes, and submit a pull request.

## Acknowledgments
Special acknowledgment to **Tuvi Etzion**, professor of computer science at **Technion – Israel Institute of Technology**, for his invaluable advice and support throughout this project. His expertise significantly contributed to the successful completion of this work

## LICENSE
This project is licensed under Apache License 2.0. For more information, see the LICENSE file in this repository.

## Contact
name       |                   email                      |    github
-----------|----------------------------------------------|----------------------------------------
Joan Jozen | [joan.jozen@gmail.com](mailto:joan.jozen@gmail.com) | [joannj35](https://github.com/joannj35)
Razan Dally| [razandally@gmail.com](mailto:razandally@gmail.com) | [RazanDally](https://github.com/RazanDally)
