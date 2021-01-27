#VPlr

Video Data Player - application for synchronous video playback and data plotting.

## The Idea

Basic idea for this app arise from real life need to watch the fast cam video and observe the recorded data in form of plots at the same time.
So in short words VPlr takes a video file and plays it drawing plot from csv file.
The trick is that both can be synchronized by defining video frame and data value.

## Usage

At the heart of the development VPlr follows the unix principal approach - single tool to do single task. But do it well.
With this in mind I decided to not introduce the GUI and any other complicated interface - but to base the interaction on the plain text config file.

This app need to be executed with a single command line parameter which is a name of such config file.
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

App contains three windows **Video** **Plot** and **Zoom Plot**
All controls are based in keyboard and are the same independently of which window is active. 

The keys are as follows:

 - <Esc> Exit and close all windows
 - ; playback forward
 - h playback backward
 - j single frame back
 - l single frame forward
 - k pause
 - [ zoom out 
 - ] zoom in
 - e set the current position as max
 - r release the max position lock
 - Q restart playback from beginning
 - (+) speed up playback
 - (-) slow down playback

When playing the video certain amount of frames are loaded up to memory. This allows for smooth forward and backward playback within this frame range. The range is marked on the main plot by two violet(kind) lines. This range travels along with the played video - and it might happen that the range will pass interesting parts and will not allow to play back. To fix it Q key (shift+q) can be press to start playback from beginning.
To limit the position of the cached frames range (lock it) the e key can be press. Key r release the lock. 
 

## Config file

### Example
```
# Data Player config file. Order of non commented lines is important!
# (0)vide file to load
/path_to_video_file/video_file_name.mp4
# (1)csv data file to load; (1.1) number of datafiles lines to skip
/path_to_csv_file/csv_file_name.csv; 10
# (2)columns to be plotted on general plot counting form 1
-5;(+3,-4);7;1
# (3)columns to be plotted on zoom plot counting form 1
-5;(+3,-4)
# (4)video sync frame - video frame thet syncs with the video sync signal
500
# (5) video sync signal column counting from 1 - when the data in this column rise from 0 to 1 it is the video sync position
9
# End of file

```
Parameters description (identified by the numbers):

- 0 - the path and filename for the video file. Path to files can be relative to the working directory or absolute.
- 1 - the path and filename of the csv datafile, the data need to be delimited by semicolon (;)
- 1.1 - the number of lines from datafile to skip when loaded (to skip headers etc.)
- 2 - numbers of columns of the datafile to be displayed on main plot view. Counting from 1 (the first column is 1). 
    - If the column number is set as negative the data will be multiply by -1 (inverted)
    - If instead of a number a set of numbers in brackets and separated by commas are given, the tool will create the plot by sum of the listed columns with the respecting the signs. I.e. (3,-4) makes value adding data from column 3 and inverted column 4, which virtually is subtracting column 4 form 3: (3,-4) = 3 + (-4).
- 3 - just as above but for the zoomed view window.
- 4 - the number of video frame that will be synced with csv data column.
- 5 - the data column of the sync to video signal (the first rise slope of this signal is used).



