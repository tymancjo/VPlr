#VPlr

Video Data Player - application for synchronous video playback and data plotting.

## The Idea

Basic idea for this app arise from real life need to watch the fast cam video and observe the recorded data in form of plot at the same time.
So in short words VPlr takes a video file and plays it drawing plot from csv file.
The trick is that both can be synchronized by defining vireo frame and data value.

## Usage

This app need to be executed with a single command line parameter which is a name of config file name.
The config file is a text file of a predefined structure.
In linux systems the execution command can look like:

```
./VPlr config.txt
```
In windows

```
VPlr.exe config.txt
```

The csv file need to use the semicolon `;` as column separator.

## App interface

The app is controlled by keyboard. The keys are as follows:
 - ; playback forward
 - h playback backward
 - j single frame back
 - l single frame forward
 - k pause
 - [ zoom out 
 - ] zoom in
 - e set the current position as max
 - r release the max position lock
 - Q restart playback from beggining
 - + speed up playback
 - - slow down playback
 

## Config file

### Example
```
# Data Player config file. Order of non commented lines is important!
# (0)vide file to load
/path_to_video_file/video_file_name.mp4
# (1)csv data file to load
/path_to_csv_file/csv_file_name.csv
# (2)columns to be plotted on general plot counting form 1
-5;6;7;1
# (3)columns to be plotted on zoom plot counting form 1
-5;4
# (4)video sync frame - video frame thet syncs with the video sync signal
500
# (5) video sync signal column counting from 1 - when the data in this column rise from 0 to 1 it is the video sync position
9
# End of file

```

