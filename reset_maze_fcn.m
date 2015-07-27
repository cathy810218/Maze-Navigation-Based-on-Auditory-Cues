function reset_maze_fcn(fig_obj,eventDat)
%Handles a maze reset triggered by the Play Again button
%Retores the maze to a new state with a randomly generated source
% destination, and begins the user at position {2,2}

    %import global variables
    global currentRow
    global currentCol
    global gameOver
    global msg
    global playAgain
    global MAZE_MATRIX
    global destRow
    global destCol
    global numMoved
    global moveCounter
    global soundPlayer
    global height
    global width
    global facingDirection

    prevRow = currentRow;
    prevCol = currentCol;

    % hard code to always start in the top left
    currentRow = 2;
    currentCol = 2;

    % reset the move counter
    numMoved = 0;
    % reset the GUI elements above the maze
    set(playAgain,'enable','off');
    set(msg,'String','Good Luck!');
    set(moveCounter,'String','0');
    % reset the gameOver flag 
    gameOver = 0;

    %random generator for new destination coodinates
    newDestCol = randi(width);
    newDestRow = randi(height);
    dRow = newDestRow - currentRow;
    dCol = newDestCol - currentCol;
    dist_SS2user = sqrt(dRow^2 + dCol^2);

    while (MAZE_MATRIX(newDestRow,newDestCol) && dist_SS2user > 5) % when its not 0
        newDestCol = randi(width);
        newDestRow = randi(height);
    end

    %update the destination coordinates
    destRow = newDestRow;
    destCol = newDestCol;
    
    %reset facing direction to down
    facingDirection = 0;
    
    %update the parameters sent to C and update the GUI
    updateSoundParams();
    updateGUI(prevRow, prevCol);
    %play the sound immediately when a new game is started
    play(soundPlayer);
end