#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include "opencv4/opencv2/opencv.hpp"


//////////////////////////////
// the function definitions //
//////////////////////////////

struct results {
    // for returning more parameters from function
    bool sucess;
    std::vector<std::vector<std::string>> strMatrix;
    std::vector<std::vector<float>> fMatrix;
};

results readFileAsText(std::string sFilename,  char deliminator = ';')
    {
        results output;

        std::ifstream f(sFilename);
        if (!f.is_open()){
            output.sucess = false;
            return output;
        }

        while (!f.eof())
        {   std::string line;
            std::getline(f, line);

            if (!line.empty()){
                if (line.at(0) != '#'){
                    std::istringstream iss(line);
                    std::string token;
                    std::vector < std::string> lins;
                    while(std::getline(iss, token, deliminator)){
                        boost::trim(token);
                        boost::replace_all(token,",",".");
                        lins.push_back(token);
                    }

                    output.strMatrix.push_back(lins);
        }}}
        output.sucess = true;
        return output;
    };


results parseCsvData(std::vector<std::vector<std::string>> inputText, int startRow, int cut_columns){

    results output;
    // first I need to figure out how many columns in data
    // so the idea is to make the size first
    // so I'm adding 0 to each column vector

    for (int col=0; col < inputText[startRow].size() - cut_columns; col++){
        std::vector<float> C;
        C.push_back(0);
        output.fMatrix.push_back(C);
        C.pop_back();
    }

    for (int row=startRow; row < inputText.size(); row++){
        for (int col=0; col < inputText[row].size() - cut_columns; col++){
            std::stringstream iss( inputText[row][col] );
            float val;
            if (iss >> val) {
                output.fMatrix[col].push_back(val);
            } else {
                output.sucess = false;
                return output;
            }}}
    output.sucess = true;
    return output;
};

std::vector<float> normalizeFloatVec(std::vector<float> inputVector){

    std::vector<float> M;
    float max = 0;

    for (int el=0; el < inputVector.size(); el++){
        max = (abs(inputVector[el]) > max)? abs(inputVector[el]) : max;
    }
    for (int el=0; el < inputVector.size(); el++){
        if (max != 0)
            M.push_back(inputVector[el] / max);
        else
            M.push_back(0);
    }
    M[0] = max; // Memorizing the max in the added cell 0
    return M;
};

int findIndex(std::vector<float> V, float treshold, int start=0){
    for (int i = start; i < V.size(); i++){
        if (V[i] >= treshold)
            return i;
    }
    return 0;
}

std::vector<std::string> splitLine(std::string line, char deliminator = ';'){
    boost::trim(line);
    std::istringstream iss(line);
    std::string token;
    std::vector < std::string> lins;

    while(std::getline(iss, token, deliminator)){
        boost::trim(token);
        boost::replace_all(token,",",".");
        lins.push_back(token);
    }
    return lins;
}


static void mouseCall(int event, int x, int y, int flags, void* img){
    
    if (event == cv::EVENT_MOUSEMOVE) {
        //std::cout << "event mouse move"<< std::endl; 
        //std::cout << "x: "<< x << " y: " << y << std::endl; 
    return;
}}

int str2int(std::string input_text){
    std::stringstream iss(input_text);
    int val;
        if (iss >> val) {
           return val;
        } else return 0;
}


///////////////////////////////////////////////////////////////////////////////
// the main loop //
///////////////////

int main(int argc, char *argv[])
{
    // handling the line parameters
    if (argc < 2){
        std::cout << "[ERROR] No config file as line parameter..." << std::endl;
        return -1;
    }

    std::string config_file = argv[1];
    results fOutput;
    fOutput = readFileAsText(config_file, ';' );

    if (!fOutput.sucess){
        std::cout << "[ERROR] Config file reading error..." << std::endl;
        return -1;
    }
    
    // reading the config parameters
    // all columns count in config starts from 1 
    // just for easier human readability
    std::string input_video_file = fOutput.strMatrix[0][0];
    std::string input_csv_file = fOutput.strMatrix[1][0];

    // reading the data start row
    std::cout << fOutput.strMatrix[1].size() << std::endl;
    int skip_rows = 9;
    if(fOutput.strMatrix[1].size() > 1){
        skip_rows = str2int(fOutput.strMatrix[1][1]);
    }
    std::cout << skip_rows << std::endl;


    // reading which data columns to zoom plot from config file
    std::vector<std::vector<int>> to_plot;
    for (int i=0; i < fOutput.strMatrix[2].size(); i++){
        std::vector<int> line_data;
        std::string this_str = fOutput.strMatrix[2][i];

        if (this_str.at(0) == '('){
            // handling the math input 
            // cleaning the bracket
            boost::replace_all(this_str,"(","");
            boost::replace_all(this_str,")","");

            std::istringstream iss(this_str);
            std::string token;

            while(std::getline(iss, token, ',')){
                boost::trim(token);
                boost::replace_all(token,",",".");
                line_data.push_back(str2int(token));
            }

            to_plot.push_back(line_data);

        } else {
            int val = str2int(this_str);
            line_data.push_back(val);
            to_plot.push_back(line_data);
        }
    }

    // reading which data columns to zoom plot from config file
    std::vector<std::vector<int>> to_zoom_plot;
    for (int i=0; i < fOutput.strMatrix[3].size(); i++){
        std::vector<int> line_data;
        std::string this_str = fOutput.strMatrix[3][i];

        if (this_str.at(0) == '('){
            // handling the math input 
            // cleaning the bracket
            boost::replace_all(this_str,"(","");
            boost::replace_all(this_str,")","");

            std::istringstream iss(this_str);
            std::string token;

            while(std::getline(iss, token, ',')){
                boost::trim(token);
                boost::replace_all(token,",",".");
                line_data.push_back(str2int(token));
            }

            to_zoom_plot.push_back(line_data);

        } else {
            int val = str2int(this_str);
            line_data.push_back(val);
            to_zoom_plot.push_back(line_data);
        }
    }

    // Sanity check for the incoming data
    if (to_plot.size() < 1 || to_zoom_plot.size() < 1){
        std::cout << "[ERROR] No columns to plot identified in config...";
        return -4;
    }

    // reading the video sync frame from config
    int video_start_frame = 500;
    {
    std::stringstream iss(fOutput.strMatrix[4][0]);
        int val;
            if (iss >> val) {
                video_start_frame = val;
            }
    }

    // reading the video sync signal column
    int video_sync_idx = 0;
    {
    std::stringstream iss(fOutput.strMatrix[5][0]);
        int val;
            if (iss >> val) {
                video_sync_idx = val - 1;
            }
    }

    cv::VideoCapture cap(input_video_file);
    int total_video_frames =  cap.get(cv::CAP_PROP_FRAME_COUNT);

    if(!cap.isOpened())  // check if we succeeded
        return -1;

    // the csv file load drill
    std::vector <std::vector<std::string>> fileData;
    std::vector <std::vector<float>> fileFloatData;

    fOutput = readFileAsText(input_csv_file, ';' );

    if(!fOutput.sucess){
        std::cout << "[ERROR] issue with data file..." << std::endl;
        return -2;
        }

    fileData = fOutput.strMatrix;

    // plotting the first datalines 
    for (int i=0; i < 16; i++){
            for(int j=0; j < fileData[i].size(); j++){
                std::cout << ":" << fileData[i][j];
            }
            std::cout << "|" << std::endl;
    }

    // auto checking if need to skip last line element
    int skip_cols = 1;
    {
    std::stringstream iss(fileData[skip_rows][fileData[skip_rows].size()-1]);
        float val;
            if (iss >> val) {
                skip_cols = 0;
            }
    }

    fOutput = parseCsvData(fileData, skip_rows, skip_cols);

    if(!fOutput.sucess){
        std::cout << "[ERROR] issue with data parse..." << std::endl;
        return -3;
        }
    fileData.clear();

    fileFloatData = fOutput.fMatrix;
    int columns = fileFloatData.size();

    // preparing the zoom display array // ///////////////////////////////////
    std::vector <std::vector<float>> mainFloatData;
    for (int idx=0; idx < to_plot.size(); idx++){
            // in case of multiple indexes we need to do the math
            std::vector<float> new_data_vec;

            for (int pt=0; pt < fileFloatData[0].size(); pt++){
                // looping over all datapoints
                float new_data_point = 0.0f;
                
                for (int i=0; i<to_plot[idx].size(); i++){

                    int p = abs(to_plot[idx][i]);
                    int mp = to_plot[idx][i] / p;
                    p = p - 1; // as the user input counts from 1

                    new_data_point += mp * fileFloatData[p][pt];
                }
                new_data_vec.push_back(new_data_point);
            }
            mainFloatData.push_back(new_data_vec);
        }

    // normalizing  zoom data vector
    for (int col=0; col < mainFloatData.size(); col++){
        std::vector<float> V;
        V = normalizeFloatVec(mainFloatData[col]);
        mainFloatData[col] = V;
        }
    // ///////////////////////////////////////////////////////////////////////


    // preparing the zoom display array // ///////////////////////////////////
    std::vector <std::vector<float>> zoomFloatData;
    for (int idx=0; idx < to_zoom_plot.size(); idx++){
            // in case of multiple indexes we need to do the math
            std::vector<float> new_data_vec;

            for (int pt=0; pt < fileFloatData[0].size(); pt++){
                // looping over all datapoints
                float new_data_point = 0.0f;
                
                for (int i=0; i<to_zoom_plot[idx].size(); i++){

                    int p = abs(to_zoom_plot[idx][i]);
                    int mp = to_zoom_plot[idx][i] / p;
                    p = p - 1; // as the user input counts from 1

                    new_data_point += mp * fileFloatData[p][pt];
                }
                new_data_vec.push_back(new_data_point);
            }
            zoomFloatData.push_back(new_data_vec);
        }

    // normalizing  zoom data vector
    for (int col=0; col < zoomFloatData.size(); col++){
        std::vector<float> V;
        V = normalizeFloatVec(zoomFloatData[col]);
        zoomFloatData[col] = V;
        }
    // ///////////////////////////////////////////////////////////////////////


    // normalizing data vector
    for (int col=0; col < columns; col++){
        std::vector<float> V;
        V = normalizeFloatVec(fileFloatData[col]);
        fileFloatData[col] = V;
        }

    // /////////////////////////////////////////////////////////////////
    // from here the fileFloatData vector have all the data prepared  //
    // /////////////////////////////////////////////////////////////////

    // Detecting data sample rate
    float sample_rate;
    sample_rate = 1.0f / (fileFloatData[0][0]*(fileFloatData[0][2] - fileFloatData[0][1]));
    std::cout << "Detected data sample rate: " << sample_rate << " [Sampl/s]"<< std::endl;


    int videoSyncIndex = findIndex(fileFloatData[video_sync_idx], 1.0f, 1);
    std::cout << "video sync index " << videoSyncIndex << std::endl;

    // zoom plot
    int plotWpx = 800;
    int plotHpx = 800;
    int pixelYpos = plotHpx / 2;

    cv::Mat plotFrame(plotHpx, plotWpx, CV_8UC3, cv::Scalar(0,0,0));

    // full plot 
    // vector with the plots to draw
    int plots_n = mainFloatData.size();

    int fullPlotWpx = 800;
    int fullPlotHpx = 120 * plots_n;

    cv::Mat fullPlotFrame(fullPlotHpx + 50, fullPlotWpx, CV_8UC3, cv::Scalar(0,0,0));
    fullPlotFrame = cv::Scalar(0,0,0);

    // figuring out the  X step for data plot
    int dataStep = 1;

    if (mainFloatData[0].size() > fullPlotWpx){
       dataStep = mainFloatData[0].size() / fullPlotWpx;
    }
    int dXpx = fullPlotWpx * dataStep / mainFloatData[0].size();
    dXpx = (dXpx < 1)? 1:dXpx;

    // figuring out the Y position and scale
    float plotYscale = (fullPlotHpx - 10 * plots_n - 10) / (2.0f * plots_n);

    int posY = 10 - plotYscale;

        for (int idy=0; idy < mainFloatData.size(); idy++){

        posY += 2 * plotYscale + 10;

        // X axis line
        cv::line(fullPlotFrame, cv::Point(0,posY), cv::Point(fullPlotWpx, posY),cv::Scalar(255,255,255),1,8,0);

        // plotting the whole plot-lines
            int posX = 0;
            int posX1 = 0;

            for (int idx=1; idx < mainFloatData[idy].size()-dataStep; idx += dataStep){
                // starting from one as the 0 is the added max value
                posX1 += dXpx;

                float pointVal0 = posY - (int)(mainFloatData[idy][idx] * plotYscale);
                float pointVal1 = posY - (int)(mainFloatData[idy][idx + dataStep] * plotYscale);

                cv::line(fullPlotFrame, cv::Point(posX, pointVal0), cv:: Point(posX1, pointVal1),cv::Scalar(0,255,0),1,8,0);
                posX = posX1;
            }
    }

    // adding the keys info text
    std::string keys_desc = "<--(H) |<-(J) | [K] | (L)-> | (;)-->       (-)/(+) change play speed";
    cv::putText(fullPlotFrame, keys_desc, cv::Point((fullPlotWpx - 7*keys_desc.size()) / 2,fullPlotHpx+10), cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(200,200,200),1,cv::LINE_AA);
    keys_desc = "(E) set current frame as end lock (R) release end lock  ([) zoom in  (]) zoom out";
    cv::putText(fullPlotFrame, keys_desc, cv::Point((fullPlotWpx - 7*keys_desc.size()) / 2,fullPlotHpx+30), cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(200,200,200),1,cv::LINE_AA);


    // output some info
    std::cout << "Data step: " << dataStep << std::endl;
    std::cout << "Data scale: " << plotYscale << std::endl;
    std::cout << "dXpx: " << dXpx << std::endl;

    // preparing frame buffers
    cv::Mat videoFrame;
    std::vector <cv::Mat> frame_buffer;
    cv::Mat fullPlot;

    // skipping the video to 500 frame
    cap.set(cv::CAP_PROP_POS_FRAMES, video_start_frame);

    int klatka_stop = 30000; // just something that rather be always way more then frames
    int klatka = 0;
    int klatka_total = -1;
    int klatka_max = -1;
    int step = 1;

    bool single = !false;
    bool first_loop = true;

    int playDelay = 1;
    int plotW = 100;

    plots_n = zoomFloatData.size();
    plotYscale = (plotHpx - 10 * plots_n - 20) / (2.0f * plots_n);


    // firing the output wondoows
    cv::namedWindow("Video",cv::WINDOW_NORMAL);
    cv::namedWindow("Plot",cv::WINDOW_NORMAL);
    cv::namedWindow("Full Plot",cv::WINDOW_NORMAL);

    // binding mouse callback
    cv::setMouseCallback("Plot", mouseCall, 0);


    // ////////////////////////////////////////////////////////////////////////
    // The loop   //
    // ////////// //

    std::cout << "Starting main loop..." << std::endl;
    while (true){

        klatka_max = cv::max(klatka_total, klatka_max);

        // checking  if the play reached the stop position
        if (klatka_total >= klatka_stop && step == 1){
            step = 0;
            }

        // if we are at the end of buffer we read next frames from file
        if (step == 1 && (klatka >= frame_buffer.size()-1 || first_loop)){
            if (cap.read(videoFrame)){
                if (frame_buffer.size() > 300){
                    frame_buffer.erase(frame_buffer.begin());
                }
                frame_buffer.push_back(videoFrame);
            } else {
                step = -1;
                single = true;
            }
        }

        first_loop = false;
        // keeping the play frame on the last one from the buffer
        if(klatka > frame_buffer.size()-1){
            klatka = (frame_buffer.size()-1);
        }

        // cloning frames before putting text on it
        videoFrame = frame_buffer[klatka].clone();
        fullPlot = fullPlotFrame.clone();

        cv::putText(videoFrame, std::to_string(klatka), cv::Point(10,10), cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(200,200,200),1,cv::LINE_AA);
        cv::putText(videoFrame, std::to_string(klatka_total), cv::Point(60,10), cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(200,200,200),1,cv::LINE_AA);

        cv::putText(videoFrame, std::to_string(playDelay), cv::Point(10,30), cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(200,200,200),1,cv::LINE_AA);

        cv::putText(videoFrame, std::to_string(fileFloatData[0][0]*fileFloatData[0][videoSyncIndex + klatka + 1]*1000), cv::Point(10,60), cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(200,200,200),1,cv::LINE_AA);

        // working on the graphs

        // ////////////////////////////////////////////////////////////////////
        // zoomed frame
        plotFrame = cv::Scalar(0,0,0);

        int plotCenterIndex = videoSyncIndex + klatka_total;
        int plotStartIdx = plotCenterIndex - plotW;
        int plotEndIdx = plotCenterIndex + plotW;

        int plotdX = (int)((float)plotWpx / (2.0f * plotW));

        int pixelYpos = 10 - plotYscale;

        // looping over plots
        for (int idy=0; idy < zoomFloatData.size(); idy++){

            pixelYpos += 2 * plotYscale + 10;
            int pixelXpos0 = plotWpx / 2;

            // put the value as text
            float realValue = zoomFloatData[idy][plotCenterIndex] * zoomFloatData[idy][0];
            float textSize = 0.5f;
            cv::putText(plotFrame, std::to_string(realValue), cv::Point(10,pixelYpos - plotYscale), cv::FONT_HERSHEY_SIMPLEX,textSize,cv::Scalar(200,200,200),1,cv::LINE_AA);

            // data point dot
            cv::circle(plotFrame, cv::Point(pixelXpos0, pixelYpos - (int)(zoomFloatData[idy][plotCenterIndex] * plotYscale)), 6, cv::Scalar(0,0,255), 2, 8, 0);


            // axis lines
            cv::line(plotFrame, cv::Point(0,pixelYpos), cv::Point(plotWpx, pixelYpos),cv::Scalar(255,255,255),1,8,0);
            cv::line(plotFrame, cv::Point(plotWpx/2,0), cv::Point(plotWpx/2, plotHpx),cv::Scalar(255,255,255),1,8,0);

            // drawing right from cursor
            for (int idx = plotCenterIndex; idx < plotEndIdx+1; idx++){
                int pixelXpos1 = pixelXpos0 + plotdX;
                float pointValue0 = pixelYpos - (int)(zoomFloatData[idy][idx] * plotYscale);
                float pointValue1 = pixelYpos - (int)(zoomFloatData[idy][idx + 1] * plotYscale);

                cv::line(plotFrame, cv::Point(pixelXpos0,pointValue0), cv:: Point(pixelXpos1, pointValue1),cv::Scalar(0,255,0),1,8,0);
                pixelXpos0 = pixelXpos1;
            }

            // drawing left from cursor
            pixelXpos0 = plotWpx / 2;
            for (int idx = plotCenterIndex-1; idx > plotStartIdx-1 ; idx--){
                int pixelXpos1 = pixelXpos0 - plotdX;
                float pointValue1 = pixelYpos - (int)(zoomFloatData[idy][idx] * plotYscale);
                float pointValue0 = pixelYpos - (int)(zoomFloatData[idy][idx + 1] * plotYscale);
                cv::line(plotFrame, cv::Point(pixelXpos0,pointValue0), cv:: Point(pixelXpos1, pointValue1),cv::Scalar(0,255,0),1,8,0);
                pixelXpos0 = pixelXpos1;
            }
        }


        // full plot frame adding cursors
        int cursorX = dXpx * (1 + plotCenterIndex) / dataStep;
        int cursorXL = dXpx * (1 + plotCenterIndex - plotW) / dataStep;
        int cursorXR = dXpx * (1 + plotCenterIndex + plotW) / dataStep;
        int cursorXbuffor = dXpx * (videoSyncIndex + klatka_max - frame_buffer.size()) / dataStep;
        int cursorXbufforEnd = dXpx * (videoSyncIndex + klatka_max) / dataStep;

        cv::line(fullPlot, cv::Point(cursorX,0), cv:: Point(cursorX, fullPlotHpx),cv::Scalar(0,0,255),1,8,0);
        cv::line(fullPlot, cv::Point(cursorXL,0), cv:: Point(cursorXL, fullPlotHpx),cv::Scalar(255,0,0),1,8,0);
        cv::line(fullPlot, cv::Point(cursorXR,0), cv:: Point(cursorXR, fullPlotHpx),cv::Scalar(255,0,0),1,8,0);
        cv::line(fullPlot, cv::Point(cursorXbuffor,0), cv:: Point(cursorXbuffor, fullPlotHpx),cv::Scalar(255,0,255),1,8,0);
        cv::line(fullPlot, cv::Point(cursorXbufforEnd,0), cv:: Point(cursorXbufforEnd, fullPlotHpx),cv::Scalar(255,0,255),1,8,0);

        // displaying the windows
        cv::imshow("Video", videoFrame);
        cv::imshow("Plot", plotFrame);
        cv::imshow("Full Plot", fullPlot);

        // handling the progress and keys
        klatka += step;
        klatka_total += step;

        if(single){
            single = false;
            step = 0;
        }

        int theKey = cv::waitKey(playDelay);
        if(theKey == 27) break;
        if(theKey == 45) playDelay += 1 + playDelay / 10; // the -_ key
        if(theKey == 61) playDelay -= 1 + playDelay / 2; // the += key

        if(theKey == 101) klatka_stop = klatka_total;  // the e key
        if(theKey == 114) klatka_stop = 30000;  // the r key

        if(theKey == 104) step = -1; // the h key
        if(theKey == 107) step = 0; // the k key
        if(theKey == 59) step = 1; // the ; key

        if(theKey == 106){
             //the j key
             step = -1;
             single = true;
        }
        if(theKey == 108){
            // the l key
            step = 1;
            single = true;
        }

        if(theKey == 91) plotW -= 10; // the [ key
        if(theKey == 93) plotW += 10; // the ] key

        if (theKey == 81){
            // the Q key - reset  playback
            klatka_stop = 30000; // just something that rather be always way more then frames
            klatka = 0;
            klatka_total = -1;
            klatka_max = -1;
            step = 1;

            single = !false;
            first_loop = true;
            cap.set(cv::CAP_PROP_POS_FRAMES, video_start_frame);

            //for (int i=0; i < frame_buffer.size(); i++){
                    //frame_buffer.erase(frame_buffer.begin());
            //}
            frame_buffer.clear(); 
            // logging
            std::cout << "vstart " << video_start_frame << std::endl;
            std::cout << "buffer " << frame_buffer.size() << std::endl;
        }


        playDelay = (playDelay < 1)? 1:playDelay;
        plotW = (plotW < 10)? 10:plotW;
        plotW = (2*plotW >= plotWpx)? (plotWpx/2):plotW;

        if (klatka < 0){
            step = 0;
            single = true;
            klatka = 0;
            klatka_total++;
        }
    }
    return 0;
}
