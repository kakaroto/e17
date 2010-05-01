/* Project */
#include "../include/ecorexx/Job.h"

namespace Ecorexx {
  
Job::Job () :
  mJob (NULL),
  mCalled (false)
{  
}

Job::~Job ()
{
}

void Job::start ()
{
  mJob = ecore_job_add (Job::callback, this);
  mCalled = false;
}

void Job::cancel ()
{
  if (mJob && !mCalled)
  {
    ecore_job_del (mJob);
  }
}

void Job::callback (void *data)
{
  Job *job = static_cast <Job*> (data);
  job->signalCall.emit ();
  job->mCalled = true;
}

} // end namespace Ecorexx
