const express = require('express');
const {
  getAllCommands,
  updateCommand,
} = require('../controllers/commandController');

const router = express.Router();

router.get('/commands', getAllCommands);
router.put('/command/:id', updateCommand);

module.exports = {
  routes: router,
};
