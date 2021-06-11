#include <cassert>
#include <omp.h>
#include <iostream>
#include <tsal.hpp>
#include <tsgl.h>
#include "sortUtilities.h"

using namespace tsal;

void quickSort(ThreadSynth& synth, tsgl::Rectangle** rectangles, tsgl::Canvas* can, std::vector<int> &data, int low, int high, bool audio, bool graphics) {
  if (low < high) {
    // Partition
    int pivotValue = data[low];
    int pivot = low;
    for (int i = low + 1; i < high; i++) {
      if (audio) {
        MidiNote note = Util::scaleToNote(data[i], std::make_pair(0, MAX_VALUE), std::make_pair(C3, C7));
        synth.play(note, Timing::MICROSECOND, 40);
      }
      
      
      if (data[i] < pivotValue) {
        pivot++;
        std::swap(data[i], data[pivot]);
        if (graphics) {
          if (!audio) can->sleepFor(GRAPHIC_WAIT);
          rectangles[i]->setHeight(data[i]);
          rectangles[pivot]->setHeight(data[pivot]);
        }
      }
    }
    std::swap(data[low], data[pivot]);
    if (graphics) {
      if (!audio) can->sleepFor(GRAPHIC_WAIT);
      rectangles[low]->setHeight(data[low]);
      rectangles[pivot]->setHeight(data[pivot]);
    }
    
    quickSort(synth, rectangles, can, data, low, pivot, audio, graphics);
    quickSort(synth, rectangles, can, data, pivot + 1, high, audio, graphics);
  }
}

int main(int argc, char** argv) {
  int arg;
  
  bool audio, graphics;
  audio = false;
  graphics = false;

  while ((arg = getopt(argc, argv, "ag")) != -1) {
    switch (arg) {
      case 'a':
        audio = true;
        break;
      case 'g':
        graphics = true;
        break;
      default:
        break;
    }
  }

  // tsal setup
  Mixer mixer;
  ThreadSynth synth(&mixer);
  mixer.add(synth);
  synth.setEnvelopeActive(false);

  //tsgl setup
  tsgl::Canvas* can;
  int w = SIZE;
  int h = MAX_VALUE * 1.2;
  if (graphics) {
    can = new tsgl::Canvas(0, 0, w, h, "Quick Sort", tsgl::BLACK);
    can->start();
  }

  // Generate data
  std::vector<int> data(SIZE);
  initialize(data);
  assert( !sorted(data) );

  // rectangles
  tsgl::Rectangle** rectangles = new tsgl::Rectangle*[SIZE];
  if (graphics) {
    float start = -can->getWindowWidth() * .45;
    float width = can->getWindowWidth() * .9 / SIZE;
    for (int i = 0; i < SIZE; i++) {
      rectangles[i] = new tsgl::Rectangle(roundf(start + i * width), 0, 0, width, data[i], 0, 0, 0, tsgl::RED);
      rectangles[i]->setIsOutlined(true);
      rectangles[i]->setOutlineColor(tsgl::RED);
      can->add(rectangles[i]);
    }
  }
  
  // Sort the data
  double startTime = omp_get_wtime();
  quickSort(synth, rectangles, can, data, 0, SIZE, audio, graphics);
  double stopTime = omp_get_wtime();
  std::cout << "Time taken to sort " << SIZE << " items: "
            << stopTime - startTime << " seconds" << std::endl;
  synth.stop();
  if (graphics) can->wait();          

}
