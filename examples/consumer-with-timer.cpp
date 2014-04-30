/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

// correct way to include ndn-cxx headers
// #include <ndn-cxx/face.hpp>
// #include <ndn-cxx/util/scheduler.hpp>
#include "face.hpp"
#include "util/scheduler.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace examples {

void
onData(Face& face,
       const Interest& interest, Data& data)
{
  std::cout << "I: " << interest.toUri() << std::endl;
  std::cout << "D: " << data.getName().toUri() << std::endl;
}

void
onTimeout(Face& face,
          const Interest& interest)
{
  std::cout << "Timeout" << std::endl;
}

void
delayedInterest(Face& face)
{
  std::cout << "One more Interest, delayed by the scheduler" << std::endl;

  Interest i(Name("/localhost/testApp/randomData"));
  i.setScope(1);
  i.setInterestLifetime(time::milliseconds(1000));
  i.setMustBeFresh(true);

  face.expressInterest(i,
                       bind(&onData, ref(face), _1, _2),
                       bind(&onTimeout, ref(face), _1));
}

int
main(int argc, char** argv)
{
  try {
    // Explicitly create io_service object, which can be shared between Face and Scheduler
    boost::asio::io_service ioService;

    Interest i(Name("/localhost/testApp/randomData"));
    i.setScope(1);
    i.setInterestLifetime(time::seconds(1));
    i.setMustBeFresh(true);

    // Create face with io_service object
    Face face(ioService);
    face.expressInterest(i,
                         bind(&onData, ref(face), _1, _2),
                         bind(&onTimeout, ref(face), _1));


    // Create scheduler object
    Scheduler scheduler(ioService);

    // Schedule a new event
    scheduler.scheduleEvent(time::seconds(2),
                            bind(&delayedInterest, ref(face)));

    // ioService.run() will block until all events finished or ioService.stop() is called
    ioService.run();

    // Alternatively, a helper face.processEvents() also can be called
    // processEvents will block until the requested data received or timeout occurs
    // face.processEvents();
  }
  catch(std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  return ndn::examples::main(argc, argv);
}
