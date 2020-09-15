# Source code
The source code of the application is entirely C++

Basics of audio development are needed to understand this project.

## JUCE
We use the JUCE Framework (>= 6.0.1) as a base for the audio and midi engine.

Once JUCE downloaded, you can open the `.projucer` file to generate a makefile for the wanted architecture.

See : https://juce.com/

## Coding Style
This project uses the JUCE guidelines with a few modifications :
 - JUCE coding standards : https://juce.com/discover/stories/coding-standards
 - Member variables are prepended with the `m_` prefix
 - AAA : Almost Always Auto https://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/
 - Use classic Doxygen documentation format like the following

 ```cpp
 /**
 * @brief Get the Parameter object corresponding to the given identifier
 * 
 * @param id The identifier look for
 */
ControllableParameter getParameter(const juce::Identifier& id);
 ```