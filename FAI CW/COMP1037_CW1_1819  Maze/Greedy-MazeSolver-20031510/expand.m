% Function to take a node return an expanded array of child nodes, with f(n) values
% None of the successors should be on OBSTACLE.
% Copyright 2009-2010 The MathWorks, Inc.

function exp_array = expand(node_x, node_y, gn, xTarget, yTarget, OBSTACLE, MAX_X, MAX_Y)
    exp_array = []; % children nodes
    exp_count = 1;
    c2 = size(OBSTACLE, 1);
    for k = 1 : -1 : -1 % explore surrounding locations
        for j = 1 : -1 : -1
            if xor(k == 0 , j == 0)   % the node itself is not its successor
                s_x = node_x + k;
                s_y = node_y + j;
                if( (s_x > 0 && s_x <= MAX_X) && (s_y > 0 && s_y <= MAX_Y)) % node within array bound
                    flag = 1;                    
                    for c1 = 1 : c2
                        if(s_x == OBSTACLE(c1, 1) && s_y == OBSTACLE(c1, 2))
                            flag = 0;
                        end;
                    end; % check if a child is on OBSTACLE
                    if (flag == 1)
                        exp_array(exp_count, 1) = s_x;
                        exp_array(exp_count, 2) = s_y;
                        exp_array(exp_count, 3) = gn + distance(node_x, node_y, s_x, s_y); % cost g(n)
                        exp_array(exp_count, 4) = distance(xTarget, yTarget, s_x, s_y); % cost h(n)
                        exp_array(exp_count, 5) = exp_array(exp_count, 3) + exp_array(exp_count, 4); % f(n)
                        exp_count = exp_count + 1;
                    end
                end
            end % end of if node is not its own successor loop
        end
    end