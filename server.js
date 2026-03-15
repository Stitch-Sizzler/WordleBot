const express = require('express');
const { execFile } = require('child_process');

const app = express();
app.use(express.json());
app.use(express.static('public'));

app.post('/api/solve', (req, res) => {
    const { history } = req.body;
    
    // Pass both dictionaries to the engine
    const args = ['wordle-La.txt', 'wordle-Ta.txt']; 
    history.forEach(turn => {
        args.push(turn.guess.toLowerCase(), turn.result.toUpperCase());
    });

    execFile('./solver', args, (error, stdout, stderr) => {
        if (error) return res.status(500).json({ error: 'Solver failed' });
        
        const lines = stdout.trim().split('\n');
        let remainingWords = 2315; // Default NYT starting pool
        const recommendations = [];

        lines.forEach(line => {
            if (line.startsWith('REM ')) {
                remainingWords = parseInt(line.split(' ')[1]);
            } else if (line.includes(' ')) {
                const [word, score] = line.split(' ');
                if (word !== 'error') {
                    recommendations.push({ word, score: parseFloat(score) });
                }
            }
        });

        res.json({ recommendations, remainingWords });
    });
});

app.listen(3000, () => {
    console.log('Wordle Solver with Entropy Engine running on http://localhost:3000');
});