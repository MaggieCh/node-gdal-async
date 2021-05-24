const exec = require('util').promisify(require('child_process').exec)
const { Octokit } = require('@octokit/core')
const octokit = new Octokit({ auth: process.env.NODE_PRE_GYP_GITHUB_TOKEN })
const pkg = { repo: 'node-gdal-async', owner: 'mmomtchev' }
const workflowPublish = { ...pkg, workflow_id: 7048427, ref: 'master' };

(async () => {
  const branch = (await exec('git branch --show-current')).stdout.trim()
  process.stdout.write(`launching Github actions build on branch ${branch}`)
  workflowPublish.ref = branch
  await octokit.request('POST /repos/{owner}/{repo}/actions/workflows/{workflow_id}/dispatches', workflowPublish)

  let status
  do {
    process.stdout.write('.')
    // eslint-disable-next-line no-await-in-loop
    await new Promise((res) => setTimeout(res, 5000))
    // eslint-disable-next-line no-await-in-loop
    status = await octokit.request('GET /repos/{owner}/{repo}/actions/runs', { ...workflowPublish, per_page: 1, page: 0 })
  } while (status.data.workflow_runs[0].status !== 'completed')
  process.stdout.write('\n')
  if (status.data.workflow_runs[0].conclusion !== 'success') {
    const jobs = await octokit.request(status.data.workflow_runs[0].jobs_url)
    const failedJob = jobs.data.jobs.filter((j) => ![ 'success', 'cancelled' ].includes(j.conclusion))
    const failedStep = failedJob[0].steps.filter((j) => ![ 'success', 'cancelled' ].includes(j.conclusion))
    console.error(failedJob[0].name, failedStep[0].name, 'failed')
    throw new Error('Github actions build failed')
  }
  process.stdout.write('success')
})().catch((e) => {
  console.error(e)
  process.exit(1)
})
