#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}
*/

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    // perform vector modification under the lock
    //std::unique_lock<std::mutex> uLock(lck);
    //cv.wait(uLock, [this] { return !_vehicles.empty(); }); // pass unique lock to condition variable
    std::lock_guard<std::mutex> uLock(lck);
    std::cout << ">>>> UPDATE QUEUE |" << msg << "|" << std::endl;
    _queue.emplace_back(std::move(msg));
    cv.notify_one();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::changePhase() {
    // Simple function to switch phase to different color
    if(_currentPhase == TrafficLightPhase::red) {
        _currentPhase = TrafficLightPhase::green;
    } else if (_currentPhase == TrafficLightPhase::green) {
        _currentPhase = TrafficLightPhase::red;
    }
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method "cycleThroughPhases" should be started in a thread when
    // the public method "simulate" is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    // Drawing random uniform variables in C++
    std::uniform_real_distribution<double> dist(4000, 6000);
    std::mt19937 rng;
    // Setting random seed
    rng.seed(std::random_device{}());

    // Drawing a random number of milliseconds between 4000 and 6000
    double cycleDuration = dist(rng); // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
            std::cout << ">>>> CYCLEDUR " << cycleDuration << std::endl;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration) {
            std::cout << ">>>> UPDATE CYCLE AND ADDING TO QUEUE" << std::endl;
            changePhase();
            msgQ.send(std::move(getCurrentPhase()));

            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }
    } // eof simulation loop

}

