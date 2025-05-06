let data = {};
let currentPage = 1;
const pageSize = 20;

function prevPage() {
    if (currentPage > 1) {
        currentPage--;
        getHistory(currentPage, pageSize);
    }
}

function nextPage() {
    if (currentPage * pageSize < data.count) {
        currentPage++;
        getHistory(currentPage, pageSize);
    }
}

function getHistory(page, amount) {
    fetch('/history?id=RB&p=' + page + '&a=' + amount, {
        method: 'GET'
    }).then((response) => {
        if (response.ok) {
            // Parse the JSON response
            response.json().then((res) => {
                data = res;
                render();
            });
            // console.log(`Moved ${direction}`);
        } else {
            console.error('Error moving robot:', response.statusText);
        }
    });
}

function render() {
    // Clear the history table
    const historyTable = document.getElementById('history-table');
    historyTable.innerHTML = '';
    // console.log(data);
    // Populate the history table with the new data
    let count = 1;
    data.robots.forEach((entry) => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${count++}</td>
            <td>${entry.t}</td>
            <td>${entry.x}</td>
            <td>${entry.y}</td>
            <td style="width: 85px">${entry.h}</td>
            <td style="width: 65px">${entry.s}</td>
    `;
        historyTable.appendChild(row);
    });
}

getHistory(1, pageSize);
