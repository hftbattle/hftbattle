# Off-line development package

We have prepared an off-line C++ and Python strategy development package.
It has a kit containing libraries and files that are sufficient to develop and test strategies on your computer.

Download it using the following command ([Git](http://git-scm.com/download) must be installed):

```bash
git clone https://github.com/hftbattle/hftbattle.git
```

More on how to use Off-line Development Package:

- [Setting dependencies](#requirements)
- [Market data](#data)
- [Strategy adding](#add_strategy)
- [Strategy starting](#run_strategy)


<a id="requirements"></a>
## Dependency management

Here you can find dependency management suggestions for the following operating systems:

- [Ubuntu](#ubuntu)
- [macOS](#macos)
- [Windows](#windows)


<a id="ubuntu"></a>
### Ubuntu

Run *packages_ubuntu.sh* to install dependencies:

```bash
sudo ./packages_ubuntu.sh
```

The script installs g++, [CMake](https://cmake.org/) and Python 2:

```bash
sudo apt-get update && sudo apt-get install g++ cmake python --yes
```

<a id="macos"></a>
### macOS

Run *packages_mac.sh* to install dependencies:

```bash
./packages_mac.sh
```
The script installs:

- Apple LLVM compiler (you can find detailed installation instruction [here](http://osxdaily.com/2014/02/12/install-command-line-tools-mac-os-x/)):

  ```bash
  xcode-select --install
  ```

- [Homebrew](http://brew.sh/) package manager:

  ```bash
  /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  ```

- [CMake](https://cmake.org/), [JsonCpp](https://github.com/open-source-parsers/jsoncpp) and Python 2:

  ```bash
  brew install cmake jsoncpp python
  ```

<a id="windows"></a>
### Windows

It's essential to have the following to run local pack under Windows:

- [TDM64-GCC](http://tdm-gcc.tdragon.net) compiler at least version `5.1.0`:

  You may download it [here](http://tdm-gcc.tdragon.net/download).
  Note that you need **TDM64-gcc**.

- [CMake](https://cmake.org/) at least version `2.8.4`:

  You may install [CLion](https://www.jetbrains.com/clion/download/), or [download](https://cmake.org/) **CMake** from the official website.

- Python `2.7`:

  You may download it [here](https://www.python.org/downloads/).

Make sure that paths to *cmake.exe*, *python.exe*, *g++.exe* and *mingw32-make.exe*/*make.exe* are added to *PATH* environment variable.

<a id="data"></a>
## Market data

The package repository contains a data folder with the first several hours of market data (configuration file line "day": "2016.09.01") of the trading symbol.

That's enough to:
- make sure your code compiles successfully
- debug your strategy

To check out the core ideas of the strategy you may download dara for a complete trading day "2016.09.01" from [here](https://www.dropbox.com/s/ko2zgwt004yejts/data.zip?dl=1).

You may do it manually: just unzip the file into the root directory or use *download.py* script from the repository root.

```bash
./download.py
```

The script will download the file *data.zip* and unzip it into the *data* folder.

To test the strategy on the complete trading day you should change the value of the field `day` to "2016.09.01" in the configuration file.

<a id="run_strategy"></a>
## Execution of your strategy

This chapter describes execution of your strategy:

- [Execution in command line](#command_line)
- [CLion usage](#clion)

We will assume, that your strategy and folder are called **user_strategy**.
Read [here](#add_strategy) about creating new strategies.

<a id="command_line"></a>
### Execution in command line

You need to:

- **compile all the strategies you have**:

  ```bash
  ./build.py
  ```

  This will create libraries for your strategies in the *build* folder.
- **run a simulation**.

  ```bash
  ./run.py user_strategy
  ```

<a id="clion"></a>
### CLion usage

To launch simulation in [CLion](https:/www.jetbrains.com/clion/download) you need to:

- **specify executable file**:

  open *Run > Edit configurations*, specify executable file in the root of directory as *Executable*:

  - *mac_launcher* for macOS
  - *windows_launcher* for Windows
  - *linux_launcher* for Linux

- **specify path to your configuration file** in the arguments of command line:

  in the same tab *Run > Edit configurations* you need to fill *Program arguments* with relative path to configuration file of your strategy.
  For example:

  ```bash
  strategies/user_strategy/user_strategy.json
  ```

- **start building the project and simulating strategy** by pressing *Run* button.

<a id="add_strategy"></a>
## Strategy adding

Once you decide to try more ideas and develop more strategies, using the one strategy - one file approach may become neccessary. Let’s have a look at how to add new strategies in the off-line development package.

- [C++](#cpp)
- [Python](#python)

<a id="cpp"></a>
### Creating a new C++ strategy

#### 1. Create a strategy folder

- In the `strategies` folder create a copy of the `sample_strategy` folder
- Choose a name of your strategy.

Let’s call it *strategy_name*.

- Rename the new folder with json and cpp files inside with the *strategy_name*, *strategy_name.json*, *strategy_name.cpp* respectively.

#### 2. Register your strategy

Once the files are copied you need to edit *cpp* file.

**Register** your new strategy with the following command at the end of the cpp file:

```c++
REGISTER_CONTEST_STRATEGY(UserStrategy, strategy_name)
```

Let’s say your folder, .json-file and the strategy are called *best_strategy_ever*. You will therefore need to add the following line at the end of the file *best_strategy_ever.cpp*:

```c++
REGISTER_CONTEST_STRATEGY(UserStrategy, best_strategy_ever)
```

This is necessary due to specifics of the simulator’s dynamic linkage process of the strategies.

#### 3. Update CMake configuration

- Those of you who work in cli run the following script *build.py*:
  ```bash
  ./build.py
  ```
- Those working in CLion, go to *Tools > CMake > Reload CMake Project*.

<a id="python"></a>
### Creating a new Python strategy

Unfortunately, there is currently no way to create new folders for Python strategies.
Please write your strategy in the
 `strategies/python_strategy/python_strategy.py` file.
You may find out [here](#run_strategy) how to run the strategy.
