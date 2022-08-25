class Command {
  constructor(id, cmd, done, len, pos, response, user) {
    this.id = id;
    this.cmd = cmd;
    this.done = done;
    this.len = len;
    this.pos = pos;
    this.response = response;
    this.user = user;
  }
}

module.exports = Command;
