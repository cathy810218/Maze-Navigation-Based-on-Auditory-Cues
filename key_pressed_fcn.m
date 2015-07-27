function key_pressed_fcn(fig_obj,eventDat)
%Handles keyboard input, specifically the arrow keys used to move the user
% and the W, A, S, D, keys used to change the direction the user is facing

    %import global variables
    global MAZE_MATRIX
    global currentRow
    global currentCol
    global gameOver
    global numMoved
    global facingDirection

    %keep track of previous position before changing it
    prevRow = currentRow;
    prevCol = currentCol;

    %get the current key pressed and move user or change direciton accordingly
    currKeyPressed = get(fig_obj, 'CurrentKey');
    if (strcmp(currKeyPressed, 'rightarrow') && ~MAZE_MATRIX(currentRow, currentCol+1) && ~gameOver)
        %move RIGHT
        currentCol = currentCol + 1;
        numMoved = numMoved + 1;
    elseif (strcmp(currKeyPressed, 'leftarrow') && ~MAZE_MATRIX(currentRow, currentCol-1) && ~gameOver)
        %move LEFT
        currentCol = currentCol - 1;
        numMoved = numMoved + 1;
    elseif (strcmp(currKeyPressed, 'uparrow') && ~MAZE_MATRIX(currentRow-1, currentCol) && ~gameOver)
        %move UP
        currentRow = currentRow - 1;
        numMoved = numMoved + 1;
    elseif (strcmp(currKeyPressed, 'downarrow') && ~MAZE_MATRIX(currentRow+1, currentCol) && ~gameOver)
        %move DOWN
        currentRow = currentRow + 1;
        numMoved = numMoved + 1;
    elseif (strcmp(currKeyPressed, 'w') && ~gameOver)
        %face UP
        facingDirection = 1;
    elseif (strcmp(currKeyPressed, 'a') && ~gameOver)
        %face LEFT
        facingDirection = 2;
    elseif (strcmp(currKeyPressed, 's') && ~gameOver)
        %face DOWN
        facingDirection = 0;
    elseif (strcmp(currKeyPressed, 'd') && ~gameOver)
        %face RIGHT
        facingDirection = 3;
    end

    %update the parameters sent to C and update the GUI
    updateSoundParams();
    updateGUI(prevRow, prevCol);
end