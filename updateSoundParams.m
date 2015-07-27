function updateSoundParams()
%Updates the sound parameters:
%   direction
%   intensity
%   low pass index
%and bundles them together to be sent over to the board

    %import global variables
    global direction
    global distance
    global currentRow
    global currentCol
    global destCol
    global destRow
    global serialChannel
    global height
    global width
    global soundPlayer
    global facingDirection
    
    %if set to true, will display movement information in Command Window
    DEBUG_FLAG = 0;
    
    %calculate distance, direction to source
    deltaRow = destRow - currentRow;
    deltaCol = destCol - currentCol;
    slope = deltaRow/deltaCol;
    distance = sqrt(deltaRow^2 + deltaCol^2);
    
    %thresholds used to determine which direction the sound is coming from
    %based on the mathematics of the slope calculated using the distance
    %formula
    smallPosThresh = 0.25;
    largePosThresh = 4;
    smallNegThresh = -0.25;
    largeNegThresh = -4;
    
    %sets the direction parameter sent to C
    if (slope > largeNegThresh && slope <= smallNegThresh && deltaRow < 0)
        str = 'Front Right';
        direction = 1;
    elseif (slope > smallNegThresh && slope <= smallPosThresh && deltaCol > 0)
        str = 'Right';
        direction = 2;
    elseif (slope > smallPosThresh && slope <= largePosThresh && deltaRow >0)
        str = 'Back Right';
        direction = 3;
    elseif (slope > largePosThresh || slope == Inf || slope <= largeNegThresh && deltaRow > 0)
        str = 'Back';
        direction = 4;
    elseif (slope > largeNegThresh && slope <= smallNegThresh)
        str = 'Back Left';
        direction = 5;
    elseif (slope > smallNegThresh && slope <= smallPosThresh)
        str = 'Left';
        direction = 6;
    elseif (slope > smallPosThresh && slope <= largePosThresh)
        str = 'Front Left';
        direction = 7;
    elseif (slope > largePosThresh || slope == Inf || slope <= largeNegThresh && deltaRow<0)
        str = 'Front';
        direction = 0;
    else
        direction = NaN;
        str = 'You won';
    end
    
    %account for facing direction changes
    if (~isnan(direction))
        if (facingDirection == 3)
            %right
            direction = mod(direction-2,8);
        elseif (facingDirection == 0)
            %down
            direction = mod(direction-4,8);
        elseif (facingDirection == 2)
            %left
            direction = mod(direction-6,8);
        end
    end
    
    %the max distance the user can be from the sound source
    maxDistance = sqrt((height-2)^2 + (width-2)^2);
    %the quantization factor based on maze size
    distanceStep = maxDistance / 10;
    %set the distance parameter sent over to C
    serialDistParam = '9';
    if (distance >= maxDistance)
        serialDistParam = '9';
    elseif (distance >= maxDistance - (distanceStep * 1))
        serialDistParam = '8';
    elseif (distance >= maxDistance - (distanceStep * 2))
        serialDistParam = '7';
    elseif (distance >= maxDistance - (distanceStep * 3))
        serialDistParam = '6';
    elseif (distance >= maxDistance - (distanceStep * 4))
        serialDistParam = '5';
    elseif (distance >= maxDistance - (distanceStep * 5))
        serialDistParam = '4';
    elseif (distance >= maxDistance - (distanceStep * 6))
        serialDistParam = '3';
    elseif (distance >= maxDistance - (distanceStep * 7))
        serialDistParam = '2';
    elseif (distance >= maxDistance - (distanceStep * 8))
        serialDistParam = '1';
    else
        serialDistParam = '0';
    end
    
    %assemble the parameters to be sent as 3 characters to C
    serialParams = strcat(serialDistParam, serialDistParam, num2str(direction));
    fwrite(serialChannel, serialParams);
    
    %if the sound is not currently playing, play the sound
    %this will be triggered on every key press
    if (~soundPlayer.isplaying)
        play(soundPlayer);
    end
    
    if (DEBUG_FLAG == 1)
        'distance' 
        distance
        str
        'direction' 
        direction
        
    end
end

