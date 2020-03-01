function DFSMazeSolver(MAZE)
mazesize=size(MAZE,1);        %find maze size
MAX_X=mazesize;
MAX_Y=mazesize;

for i=1:mazesize              %find start point
    for j=1:mazesize
        if (MAZE(i,j)==3)
         startPoint=point(i,j);
        end
    end
end
xStart=startPoint.row;
yStart=startPoint.col;

for i=1:mazesize            %find end point
    for j=1:mazesize
        if (MAZE(i,j)==4)
        xTarget=i;
        yTarget=j;
        end
    end
end



OBSTACLE = [];     %put every wall into obstacle
k = 1;
for i = 1 : mazesize
    for j = 1 : mazesize
        if(MAZE(i, j) == 0||MAZE(i,j)==8||MAZE(i,j)==2||MAZE(i,j)==3)
            OBSTACLE(k, 1) = i;
            OBSTACLE(k, 2) = j;
            k = k + 1;
        end
    end
end
OBST_COUNT = size(OBSTACLE, 1);
OBST_COUNT = OBST_COUNT + 1;
OBSTACLE(OBST_COUNT, :) = [xStart, yStart];



%% add the starting node as the first node (root node) in QUEUE
% QUEUE: [0/1, X val, Y val, Parent X val, Parent Y val, g(n),h(n), f(n)]
xNode = xStart;
yNode = yStart;
QUEUE = [];
QUEUE_COUNT = 1;
NoPath = 1; % assume there exists a path
path_cost = 0; % cost g(n): start node to the current node n
goal_distance = distance(xNode, yNode, xTarget, yTarget); % cost h(n): heuristic cost of n
QUEUE(QUEUE_COUNT, :) = insert(xNode, yNode, xNode, yNode, path_cost, goal_distance, goal_distance);
QUEUE(QUEUE_COUNT, 1) = 0; % What does this do?   Reinitalize to 0,then it is explored.



totalexpand=0;
totaldiscover=0;
%% Start the search
while((xNode ~= xTarget || yNode ~= yTarget) && NoPath == 1)
    % expand the current node to obtain child nodes
    exp = expand(xNode, yNode, path_cost, xTarget, yTarget, OBSTACLE, MAX_X, MAX_Y);
    exp_count  = size(exp, 1);
    totaldiscover=totaldiscover+exp_count;
    totalexpand=totalexpand+1;
    % Update QUEUE with child nodes; exp: [X val, Y val, g(n), h(n), f(n)]
    for i = 1 : exp_count
        flag = 0;
        for j = 1 : QUEUE_COUNT
            if(exp(i, 1) == QUEUE(j, 2) && exp(i, 2) == QUEUE(j, 3))
                QUEUE(j, 8) = min(QUEUE(j, 8), exp(i, 5));
                if QUEUE(j, 8) == exp(i, 5)
                    % update parents, g(n) and h(n)
                    QUEUE(j, 4) = xNode;
                    QUEUE(j, 5) = yNode;
                    QUEUE(j, 6) = exp(i, 3);
                    QUEUE(j, 7) = exp(i, 4);
                end; % end of minimum f(n) check
                flag = 1;
            end;
        end;
        if flag == 0
            QUEUE_COUNT = QUEUE_COUNT + 1;
            QUEUE(QUEUE_COUNT, :) = insert(exp(i, 1), exp(i, 2), xNode, yNode, exp(i, 3), exp(i, 4), exp(i, 5));
        end; % end of insert new element into QUEUE
    end;
 
    % DFS: find the last node in QUEUE expand it
    for i=QUEUE_COUNT:-1:1
        if QUEUE(i,1)==1
            last=i;
            break;
        end
    end
        % set current node (xNode, yNode) to the node with minimum f(n)
        xNode = aQUEUE(last, 2);
        yNode = QUEUE(last, 3);
        path_cost = QUEUE(last, 6); % cost g(n)
        % move the node to OBSTACLE
        OBST_COUNT = OBST_COUNT + 1;
        OBSTACLE(OBST_COUNT, 1) = xNode;
        OBSTACLE(OBST_COUNT, 2) = yNode;
        QUEUE(last, 1) = 0;
        
     
  
    totaldiscover=totaldiscover+1;
    MAZE(xNode,yNode)=2;
    dispMaze(MAZE);
end;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Optimal path: starting from the last node, backtrack to
% its parent nodes until it reaches the start node
% 04-26-2005    Copyright 2009-2010 The MathWorks, Inc.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Optimal_path = [];
QUEUE_COUNT = size(QUEUE, 1);
xval = QUEUE(QUEUE_COUNT, 2);
yval = QUEUE(QUEUE_COUNT, 3);

temp = QUEUE_COUNT;         
while(((xval ~= xTarget) || (yval ~= yTarget)) && temp > 0)
    temp = temp - 1;
    xval = QUEUE(temp, 2);
    yval = QUEUE(temp, 3);
end

i = 1;
Optimal_path(i, 1) = xval;
Optimal_path(i, 2) = yval;

if ((xval == xTarget) && (yval == yTarget))
    inode = 0;
    % Traverse QUEUE and determine the parent nodes
    parent_x = QUEUE(index(QUEUE, xval, yval), 4);
    parent_y = QUEUE(index(QUEUE, xval, yval), 5);
   
    while(parent_x ~= xStart || parent_y ~= yStart)
        i = i + 1;
        Optimal_path(i, 1) = parent_x; % store nodes on the optimal path
        Optimal_path(i, 2) = parent_y;
        inode = index(QUEUE, parent_x, parent_y); % find the grandparents :)
        parent_x = QUEUE(inode, 4);
        parent_y = QUEUE(inode, 5);
    end;
    Optimal_path(i+1,1) = xStart;    % add start node to the optimal path  
    Optimal_path(i+1,2) = yStart;
     j = size(Optimal_path, 1);
    j = j - 1;

    for i = j : -1 : 1 % show the path
        MAZE(Optimal_path(i, 1),Optimal_path(i, 2))=5;
        MAZE(xTarget,yTarget)=4;
        dispMaze(MAZE)
    end;
else
    pause(1);
    h = msgbox('Oops! No path exists to the Target!', 'warn');
    uiwait(h, 5);
end
     figure('Name','Measured Data(Remove window to see the maze)','NumberTitle','off');
     c = categorical({,'number of nodes expanded','number of nodes discovered','total path cost'});
     costs = [totalexpand totaldiscover  path_cost];
     resultAStar=bar(c,costs);
     resultAStar.FaceColor = 'flat';
     resultAStar.CData(2,:) = [0 1 1];
     resultAStar.CData(3,:) = [1 1 0];
   