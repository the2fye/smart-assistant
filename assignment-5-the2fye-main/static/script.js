const API_KEY = "1234";

// LOAD TODOS
async function loadTodos() {
    const res = await fetch("/todos", {
    headers: {
        "x-api-key": API_KEY
    }
});
    const data = await res.json();

    const list = document.getElementById("todoList");
    list.innerHTML = "";

    data.forEach(([id, task]) => {
        const li = document.createElement("li");

        li.innerHTML = `
            ${task}
            <button onclick="deleteTodo(${id})">Done</button>
        `;

        list.appendChild(li);
    });
}

// ADD TODO
async function addTodo() {
    const task = document.getElementById("taskInput").value;

    await fetch("/todos", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "x-api-key": API_KEY
        },
        body: JSON.stringify({ task })
    });

    document.getElementById("taskInput").value = "";
    loadTodos();
}

// DELETE TODO
async function deleteTodo(id) {
    await fetch("/done", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "x-api-key": API_KEY
        },
        body: JSON.stringify({ id })
    });

    loadTodos();
}

// 🔥 STEP 1: LOAD NOTES
async function loadNotes() {
    const res = await fetch("/notes", {
    headers: {
        "x-api-key": API_KEY
    }
});
    const data = await res.json();

    const table = document.getElementById("notesTable");

    table.innerHTML = `
        <tr>
            <th>Text</th>
            <th>Audio</th>
        </tr>
    `;

    data.forEach(note => {
        const row = document.createElement("tr");

        row.innerHTML = `
            <td>${note[1]}</td>
            <td>
                <audio controls>
                    <source src="/audio/${note[2].replace("audio/", "")}" type="audio/wav">
                </audio>
            </td>
        `;

        table.appendChild(row);
    });
}

// AUTO REFRESH
setInterval(loadTodos, 3000);
// ❌remove auto refresh for now❌
//setInterval(loadNotes, 5000);

loadTodos();
loadNotes();


