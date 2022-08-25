'use strict';

const firebase = require('../db');
const Command = require('../models/command');
const firestore = firebase.firestore();

/*const addStudent = async (req, res, next) => {
    try {
        const data = req.body;
        await firestore.collection('students').doc().set(data);
        res.send('Record saved successfuly');
    } catch (error) {
        res.status(400).send(error.message);
    }
}*/

const getAllCommands = async (req, res, next) => {
  try {
    const commands = await firestore.collection('command');
    const data = await commands.get();
    const commandsArray = [];
    if (data.empty) {
      res.status(404).send('No command record found');
    } else {
      data.forEach((doc) => {
        if (doc.data().done == false) {
          const command = new Command(
            doc.id,
            doc.data().cmd,
            doc.data().done,
            doc.data().len,
            doc.data().pos,
            doc.data().response,
            doc.data().user
          );
          commandsArray.push(command);
        }
      });
      res.send(commandsArray);
    }
  } catch (error) {
    res.status(400).send(error.message);
  }
};

/*const getStudent = async (req, res, next) => {
  try {
    const id = req.params.id;
    const student = await firestore.collection('students').doc(id);
    const data = await student.get();
    if (!data.exists) {
      res.status(404).send('Student with the given ID not found');
    } else {
      res.send(data.data());
    }
  } catch (error) {
    res.status(400).send(error.message);
  }
};*/

const updateCommand = async (req, res, next) => {
  try {
    const id = req.params.id;
    const data = req.body;
    const command = await firestore.collection('command').doc(id);
    await command.update(data);
    res.send('Command record updated successfuly');
  } catch (error) {
    res.status(400).send(error.message);
  }
};

/*const deleteStudent = async (req, res, next) => {
  try {
    const id = req.params.id;
    await firestore.collection('students').doc(id).delete();
    res.send('Record deleted successfuly');
  } catch (error) {
    res.status(400).send(error.message);
  }
};*/

module.exports = {
  getAllCommands,
  updateCommand,
};
