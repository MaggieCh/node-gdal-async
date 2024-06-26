const ghpages = require('gh-pages')
const path = require('path')

process.stdout.write('Publishing to "gh-pages" branch... ')
ghpages.publish(path.resolve(__dirname, '../doc'), { message: 'Updated documentation [skip ci]' }, (err) => {
  if (err) {
    process.stdout.write('error\n')
    console.error(err)
    process.exit(1)
  } else {
    process.stdout.write('success\n')
    process.exit(0)
  }
})
