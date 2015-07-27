% Main module for the maze navigation game using auditory cues
% Jeff Alcock, Yi-ching Oun
% June 4, 2015
% EE 443 DSP Capstone

clear all; clc; close all;
%% Back-end (data mangement)

%binary mask representing the maze 
% 1 = wall
% 0 = path
global MAZE_MATRIX
MAZE_MATRIX = [
        1 1 1 1 1 1 1 1 1 1 1;...
        1 0 1 0 0 1 1 0 1 0 1;...
        1 0 0 0 1 0 0 0 1 0 1;...
        1 1 1 0 1 0 1 0 0 0 1;...
        1 0 0 0 0 0 1 1 1 1 1;...
        1 0 1 1 1 0 1 1 0 0 1;...
        1 0 1 0 0 0 0 0 0 1 1;...
        1 0 0 0 1 0 1 1 0 0 1;...
        1 0 1 0 1 1 1 1 0 1 1;...
        1 0 1 0 0 0 1 0 0 0 1;...
        1 1 1 1 1 1 1 1 1 1 1;...
        ];
global height
global width
[height, width] = size(MAZE_MATRIX);
%user's starting coordinates
START_POS = [2 2];
%the tiles data structure is a cell array to keep track of the maze grid axes
global tiles
%msg is a handler to the user message area
global msg
%playAgain is a handler to the play again button
global playAgain
%moveCounter is a handler to the area where the number of steps is displayed
global moveCounter
%create GUI (figure, maze matrix, messages, etc.)
[fig, tiles, moveCounter, msg, playAgain] = createGUI(size(MAZE_MATRIX,1));
%set key press handler to listen for arrow key presses
set(fig,'KeyPressFcn',@key_pressed_fcn);

global currentRow
currentRow = START_POS(1);
global currentCol
currentCol = START_POS(2);

global gameOver
gameOver = 0;

%
%manually set destination coordinates, sound parameters for first maze
%
global destRow
destRow = 10;
global destCol
destCol = 10;
%direction represents the direction to the sound source from the user's
%current position and orientation
% 0 = front
% 1 = front-right
% 2 = right
% 3 = rear-right
% 4 = rear
% 5 = rear-left
% 6 = left
% 7 = front-left
global direction
direction = 3;
%distance is the mathematical distance between the user location and sound
%source position
global distance
distance = 11.3; %11.31 for (10,10), 5.6 for (6,6)
%numMoved counts the number of spaces a user has moved in the current maze
global numMoved
numMoved = 0;
%facing direction represents the direction the user is facing
% 0 = down
% 1 = up
% 2 = left
% 3 = right
global facingDirection
facingDirection = 0;

%import the images to be used for the user (changes based on direction)
global upArrowImg
upArrowImg = imread('upArrow.jpg');
global rightArrowImg
rightArrowImg = imread('rightArrow.jpg');
global downArrowImg
downArrowImg = imread('downArrow.jpg');
global leftArrowImg
leftArrowImg = imread('leftArrow.jpg');

%% Music (import/play sounds)

[fanfare, fs] = audioread('newFan.wav');
[jungle, fs2] = audioread('newJungle.wav');
[win, fs3] = audioread('TaDa.wav');
%clip jungle to same length as fanfare and make it quieter
jungle = jungle(1:967680)/5;
%clip an extra second from both to make transition smoother
fanfare = fanfare(1:923580);
jungle = jungle(1:923580);
%make the win sound quieter
win = win./2;

%The final sound is composed of the jungle sound in the left channel and
%the fanfare in the right channel. This is to allow C to process the two
%separately. Both are eventually played in both channels when filtering is
%complete.
final(:,1) = jungle;
final(:,2) = fanfare;

%create sound players to play/stop audio
global soundPlayer
soundPlayer = audioplayer(final, fs);
global winPlayer
winPlayer = audioplayer(win, fs);

%% C interface (send packaged parameters to the board)

%open serial channel to transmit data to C
delete(instrfindall);
global serialChannel
serialChannel = serial('COM4', 'BaudRate',115200);
set(serialChannel,'InputBufferSize',20000);
fopen(serialChannel);

%write random data to initialize serial sequence
%(we don't know why, but it seems to need a whole bunch of writes at first
%to make everything work)
for i = 1:80
    fwrite(serialChannel, '2');
end
%write flag to indicate end of initialization
fwrite(serialChannel, '*');

%% Back-end

%initialize GUI to beginning state
initializeGUI(MAZE_MATRIX, START_POS);
