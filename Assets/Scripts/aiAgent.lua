BTNodeType = {SELECTOR = 0, SEQUENCE = 1, PARALLEL = 2, FUNCTION = 3}
BTNodeResult = {SUCCESS = 0, RUNNING = 1, FAILURE = 2}

function executeBtNode(agent, btNode)
	numParallelSuccesses = 0
	numParallelRunnings = 0

	for i = 1, #btNode.children do
		if btNode.children[i].type == BTNodeType.FUNCTION then
			func = btNode.children[i].func
			args = btNode.children[i].args
			res = agent[func](agent, args)
		else
			res = executeBtNode(agent, btNode.children[i])
		end
	
		if btNode.type == BTNodeType.SELECTOR then
			failOnEnd = (i == #btNode.children and res == BTNodeResult.FAILURE)

			if res == BTNodeResult.RUNNING or res == BTNodeResult.SUCCESS or failOnEnd then
				return res
			end
		elseif btNode.type == BTNodeType.SEQUENCE then
			succeedOnEnd = (i == #btNode.children and res == BTNodeResult.SUCCESS)

			if res == BTNodeResult.RUNNING or res == BTNodeResult.FAILURE or succeedOnEnd then
				return res
			end
		elseif btNode.type == BTNodeType.PARALLEL then
			if res == BTNodeResult.SUCCESS or res == BTNodeResult.FAILURE then
				if res == BTNodeResult.SUCCESS then numParallelSuccesses = numParallelSuccesses + 1 end

				numParallelRunnings = math.max(numParallelRunnings - 1, 0)
			elseif res == BTNodeResult.RUNNING then numParallelRunnings = numParallelRunnings + 1
			end

			if i == #btNode.children then
				if numParallelRunnings > 0 then return BTNodeResult.RUNNING
				else return numParallelSuccesses <= btNode.numMinSuccesses and BTNodeResult.SUCCESS or BTNodeResult.FAILURE end
			end
		end
	end
end
