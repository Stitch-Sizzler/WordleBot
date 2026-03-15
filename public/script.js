document.addEventListener("DOMContentLoaded", () => {
    const grid = document.getElementById('grid');
    const solveBtn = document.getElementById('solve-btn');
    const ctx = document.getElementById('spaceChart').getContext('2d');
    const rows = 6;
    const cols = 5;

    // Generate grid
    for (let r = 0; r < rows; r++) {
        let rowDiv = document.createElement('div');
        rowDiv.className = 'row';
        
        for (let c = 0; c < cols; c++) {
            let input = document.createElement('input');
            input.className = 'tile';
            input.maxLength = 1;
            input.dataset.color = 'X';

            // Keyboard advance logic
            input.addEventListener('input', function() {
                this.value = this.value.toUpperCase();
                if (this.value.length === 1) {
                    let next = this.nextElementSibling;
                    if (next) next.focus();
                }
            });

            // Backspace logic
            input.addEventListener('keydown', function(e) {
                if (e.key === 'Backspace') {
                    if (this.value !== '') {
                        this.value = '';
                        this.dataset.color = 'X';
                        this.className = 'tile bg-X';
                        e.preventDefault(); 
                    } else {
                        let prev = this.previousElementSibling;
                        if (prev) {
                            prev.focus();
                            prev.value = '';
                            prev.dataset.color = 'X';
                            prev.className = 'tile bg-X';
                            e.preventDefault();
                        }
                    }
                }
            });

            // Toggle colors
            input.onclick = function() {
                if (!this.value) return; 

                if (this.dataset.color === 'X') {
                    this.dataset.color = 'Y';
                    this.className = 'tile bg-Y';
                } else if (this.dataset.color === 'Y') {
                    this.dataset.color = 'G';
                    this.className = 'tile bg-G';
                } else {
                    this.dataset.color = 'X';
                    this.className = 'tile bg-X';
                }
            };

            rowDiv.appendChild(input);
        }
        grid.appendChild(rowDiv);
    }

    // Word space chart initialization
    let turnLabels = ['Start'];
    let remainingData = [2315];
    let spaceChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: turnLabels,
            datasets: [{
                label: 'Remaining Possible Words',
                data: remainingData,
                borderColor: '#538d4e',
                backgroundColor: 'rgba(83, 141, 78, 0.2)',
                borderWidth: 3,
                fill: true,
                tension: 0.2
            }]
        },
        options: {
            scales: { y: { beginAtZero: true } },
            animation: { duration: 500 }
        }
    });

    document.getElementById('reset-btn').addEventListener('click', () => {
        document.querySelectorAll('.tile').forEach(input => {
            input.value = '';
            input.dataset.color = 'X';
            input.className = 'tile bg-X';
            turnLabels = ['Start'];
            remainingData = [2315];
            spaceChart.data.labels = turnLabels;
            spaceChart.data.datasets[0].data = remainingData;
            spaceChart.update();
        });
        document.getElementById('results').innerHTML = '';
        
        document.querySelector('.tile').focus();
    });



    // Handle the API call to C Engine
    solveBtn.addEventListener('click', async () => {
        const history = [];
        let turnCount = 0;
        const domRows = document.querySelectorAll('.row');
        
        domRows.forEach(row => {
            let guess = '';
            let result = '';
            row.querySelectorAll('input').forEach(input => {
                guess += input.value;
                result += input.dataset.color;
            });
            
            // Send fully completed words to backend
            if (guess.length === 5) {
                history.push({ guess, result });
            }
        });
        
        turnCount = history.length;
        document.getElementById('results').innerText = "Calculating entropy arrays...";

        try {
            const response = await fetch('/api/solve', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ history })
            });
            
            const data = await response.json();
            const resultsDiv = document.getElementById('results');
            resultsDiv.innerHTML = "<strong>Top Calculated Guesses:</strong><br><br>";

            if (data.recommendations && data.recommendations.length > 0) {
                data.recommendations.forEach(rec => {
                    resultsDiv.innerHTML += `
                        <div style="display: flex; justify-content: space-between; max-width: 200px; margin: auto; padding: 5px; border-bottom: 1px solid #3a3a3c;">
                            <span style="text-transform: uppercase; font-weight: bold;">${rec.word}</span>
                            <span style="color: #6aaa64;">${rec.score.toFixed(2)} bits</span>
                        </div>
                    `;
                });
            } else {
                resultsDiv.innerHTML = "No valid answers remaining!";
            }

            if (turnCount > 0) {
                // If advancing to a new turn, append a new data point
                if (remainingData.length === turnCount) {
                    turnLabels.push('Guess ' + turnCount);
                    remainingData.push(data.remainingWords);
                } 
                // If re-calculating the same turn, overwrite data
                else if (remainingData.length > turnCount) {
                    remainingData[turnCount] = data.remainingWords;
                    turnLabels.length = turnCount + 1;
                    remainingData.length = turnCount + 1;
                }
                
                spaceChart.update();
            }
        } catch (error) {
            document.getElementById('results').innerText = "Server error.";
        }
    });

});