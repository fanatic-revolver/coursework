load data.mat
m = size(X, 2);
%y(1:500) = 0;       % 0 is indicate as 10
%y = full(ind2vec(y' + 1));
X = X';
% Randomly select 100 data points to display
sel = randperm(size(X, 1));
sel = sel(1:100);
figure(2);
displayData(X(sel, :));
X = X';

% fprintf('Program paused. Press enter to continue.\n');
% pause;

% create a neural network
net =  patternnet(30);
net.trainParam.max_fail=50; 


% divided into training, validation and testing simulate
net.divideParam.trainRatio = 0.9;
% net.divideParam.valRatio = 0;
net.divideParam.testRatio = 0.1;


rand_indices = randperm(size(X, 2));

trainData = X(:, rand_indices(1:2950));
trainLabels = y(:, rand_indices(1:2950));
testData = X(:, rand_indices(2951:end));
testLabels = y(:, rand_indices(2951:end));

% train a neural network
net = train(net, trainData, trainLabels);

% show the network
view(net);

preds = net(testData);
est = vec2ind(preds) - 1;
tar = vec2ind(testLabels) - 1;

% find percentage of correct classifications
accuracy = 100*length(find(est==tar))/length(tar);
fprintf('Accuracy rate is %.2f\n', accuracy);

% confusion matrix
plotconfusion(testLabels, preds)

save('nn_model.mat','net');
% Pause
fprintf('Program paused. Press enter to continue.\n');
pause;
    
%  To give you an idea of the network's output, you can also run
%  through the examples one at the a time to see what it is predicting.
%  Randomly permute examples
rp = randperm(m);

for i = 1:m
    % Display 
    fprintf('\nDisplaying Example Image\n');
    displayData(X(:, rp(i))');

    pred = vec2ind(net(X(:, rp(i)))) - 1;
    lbl = vec2ind(y(:, rp(i))) - 1;
    if pred==0
        predname='cat';
    elseif pred==1
        predname='dog';
    else 
        predname='panda';
    end
     
        if lbl==0
            lblname='cat';
        elseif lbl==1
            lblname='dog';
        else 
            lblname='panda';
        end
    txtPred = strcat('Neural Network Prediction: ', predname);
    txtLabl = strcat('Label: ', lblname);
    text(2, 2, txtPred, 'FontSize', 15, 'Color', 'red');
    text(2, 3, txtLabl, 'FontSize', 15, 'Color', 'green');
    % fprintf('\nNeural Network Prediction: %d\n', pred);
    
    % Pause
    fprintf('Program paused. Press enter to continue.\n');
    pause;
end

